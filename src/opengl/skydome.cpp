/*
 * Skydome.cpp
 *
 *  Created on: Jun 19, 2011
 *      Author: markus
 */

#include <opengl/skydome.hpp>

#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <string.h>
#include <iostream>

namespace ogl {

/* Texture layout:
 * ------------------------
 * | big glow  |   glow   |
 * ------------------------
 * |   halo    |  streak  |
 * ------------------------
 */
static const float flare_uv[4][4][2] = {
		{{ 0.0f, 0.0f }, { 0.0f, 0.5f }, { 0.5f, 0.5f }, { 0.5f, 0.0f }}, //< big glow
		{{ 0.5f, 0.0f }, { 0.5f, 0.5f }, { 1.0f, 0.5f }, { 1.0f, 0.0f }}, //< glow
		{{ 0.0f, 0.5f }, { 0.0f, 1.0f }, { 0.5f, 1.0f }, { 0.5f, 0.5f }}, //< halo
		{{ 0.5f, 0.5f }, { 0.5f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.5f }}  //< streak
};

Skydome::Skydome()
	: m_list(0),
	  m_radius(1000.0f * 0.01f)
{
	m_horizon = Vec4f(0.9f, 0.7f, 0.7f, 1.0f);
}

Skydome::Skydome(float radius, const std::string& clouds, const std::string& shader, const std::string& fileName, const std::string& flares)
	: m_list(0),
	  m_radius(1000.0f * 0.01f)
{
	load(radius, clouds, shader, fileName, flares);
}

Skydome::~Skydome() {
	clear();
}

void Skydome::load(float radius, const std::string& clouds, const std::string& shader, const std::string& fileName, const std::string& flares)
{
	clear();
	m_shader = ShaderMgr::instance().get(shader);
	m_clouds = TextureMgr::instance().get(clouds)->m_textureID;
	m_radius = radius * 0.01f;
	m_flares = TextureMgr::instance().get(flares)->m_textureID;

	Lib3dsFile* model = lib3ds_file_load(fileName.c_str());
	if(!model)
		return;

	int faceCount = 0;
	{
		Lib3dsMesh* mesh;
		for(mesh = model->meshes; mesh != NULL; mesh = mesh->next)
			faceCount += mesh->faces;
	}

	Lib3dsMesh* mesh;
	unsigned finishedFaces = 0;
	m_list = glGenLists(1);
	glNewList(m_list, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (mesh = model->meshes; mesh != NULL; mesh = mesh->next) {

		for (unsigned curFace = 0; curFace < mesh->faces; curFace++) {

			Lib3dsFace* face = &mesh->faceL[curFace];
			for (unsigned i = 0; i < 3; i++)
				glVertex3f(
						(mesh->pointL[face->points[i]].pos[0] - 0.0f) * m_radius,
						(mesh->pointL[face->points[i]].pos[1] - 16.0f) * m_radius,
						(mesh->pointL[face->points[i]].pos[2] - 0.0f) * m_radius
				);

			finishedFaces++;
		}
	}
	glEnd();
	glEndList();
	lib3ds_file_free(model);
}

void Skydome::clear()
{
	if (m_list)
		glDeleteLists(m_list, 1);
	m_list = 0;
	m_shader = Shader();
	m_clouds = 0;
	m_time = 0.0f;
}

void Skydome::update(float dt)
{
	m_time += dt * 0.01f;
	m_delta = dt * 100.0f;

	m_horizon.x = 0.4f + fabs(sin(m_time));
	m_horizon.y = 0.4f + fabs(sin(m_time));
	//std::cout << m_horizon << std::endl;
}

static inline void drawFlare(Skydome::Flares flare, const Vec4f& color, const Mat4f& mat, const Vec3f& position, float scale)
{
	scale *= 5.0f;
	Vec3f pos = position * mat;
	float v[4][3] = {
			{ pos[0] - scale, pos[1] - scale, pos[2] },
			{ pos[0] - scale, pos[1] + scale, pos[2] },
			{ pos[0] + scale, pos[1] + scale, pos[2] },
			{ pos[0] + scale, pos[1] - scale, pos[2] }
	};
	glColor4fv(&color[0]);
	glTexCoord2fv(flare_uv[flare][0]); glVertex3fv(v[0]);
	glTexCoord2fv(flare_uv[flare][1]); glVertex3fv(v[1]);
	glTexCoord2fv(flare_uv[flare][2]); glVertex3fv(v[2]);
	glTexCoord2fv(flare_uv[flare][3]); glVertex3fv(v[3]);
}

void Skydome::render(const Camera& cam, const Vec3f& light, bool concealed)
{
	// skydome
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_clouds);

	m_shader->bind();
	m_shader->setUniform1f("time", m_time);
	m_shader->setUniform1f("invHeight", 16.0f * m_radius / 50.0f);
	m_shader->setUniform1f("invRadius", 2.0f / (10.0f * m_radius * m_radius));
	m_shader->setUniform4fv("color", &m_horizon[0]);
	m_shader->setUniform1i("s_texture_0", 0);
	m_shader->setUniform1i("s_texture_1", 0);

	glDepthMask(GL_FALSE);
	glCallList(m_list);
	glDepthMask(GL_TRUE);
	__Shader::unbind();

	// flares
	Vec3d window;
	bool inside = false;
	bool visible = false;

	visible = cam.testSphere(light, 50.0f);

	if (visible) {
		inside = !concealed;
	}

	if (inside) {
		m_fadeTime += m_delta * 0.05f;
		if (m_fadeTime > 20.0f)
			m_fadeTime = 20.0f;
	} else if (m_fadeTime > 0) {
		if (m_fadeTime > 4.5f)
			m_fadeTime = 4.5f;
		m_fadeTime -= m_delta * 0.05f;
	}

	if (!visible && m_fadeTime <= 0.0f)
		return;

	glCullFace(GL_FRONT);

	Mat4d modelview(cam.m_modelview);
	Mat4d projection(cam.m_projection);
	gluProject(light.x, light.y, light.z, modelview[0], projection[0],
			&cam.m_viewport[0], &window.x, &window.y, &window.z);

	Mat4f modelviewf(cam.m_modelview);

	Vec3f dir = modelviewf.getZ();
	dir.z *= -1.0f;
	Vec3f lightToCam = cam.m_position - light;
	Vec3f intersection = (dir * lightToCam.len()) + cam.m_position;
	Vec3f lToInt = intersection - light;
	float length = lToInt.len() * 0.3f;
	lToInt.normalize();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_flares);

	float alpha = 2.0f * m_fadeTime / 10.0f;
	Vec2f tmp(cam.m_viewport[2] * 0.5f - window[0], cam.m_viewport[3] * 0.5f - (window[1] / alpha));
	float len = tmp.len();
	float Alpha = ((cam.m_viewport[3] / len) / 5.0f) * alpha;

	glPushMatrix();
	glLoadIdentity();

	if (m_fadeTime > 0.0f) {
		Vec4f ctemp(0.6f, 0.6f, 0.8f, 20.0f * Alpha);
		glBegin(GL_QUADS);
		drawFlare(BIG_GLOW, ctemp, modelviewf, light, 32.0f);
		drawFlare(BIG_GLOW, ctemp, modelviewf, light, 32.0f);
		drawFlare(BIG_GLOW, ctemp, modelviewf, light, 32.0f);
		drawFlare(BIG_GLOW, ctemp, modelviewf, light, 32.0f);

		drawFlare(STREAK, ctemp, modelviewf, light, 32.0f);
		glEnd();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);
		drawFlare(BIG_GLOW, Vec4f(1.0f, 1.0f, 0.0f, 50.0f * Alpha), modelviewf, light, 32.0f);
		glEnd();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBegin(GL_QUADS);
		Vec3f vtemp = lToInt * 2.0f * length;
		drawFlare(STREAK, Vec4f(0.6f, 0.6f, 0.80f, 0.25f * Alpha), modelviewf, light, 32.0f);
		drawFlare(GLOW, Vec4f(0.80f, 0.80f, 1.00f, 0.5f * Alpha), modelviewf, light, 3.5f);
		/*
		drawFlare(GLOW, Vec4f(0.90f, 0.60f, 0.40f, 0.5f * Alpha), modelviewf, vtemp * 0.1000f + light, 0.60f);
		drawFlare(HALO, Vec4f(0.80f, 0.50f, 0.60f, 0.5f * Alpha), modelviewf, vtemp * 0.1500f + light, 1.70f);
		drawFlare(HALO, Vec4f(0.90f, 0.20f, 0.10f, 0.5f * Alpha), modelviewf, vtemp * 0.1750f + light, 0.83f);
		drawFlare(HALO, Vec4f(0.70f, 0.70f, 0.40f, 0.5f * Alpha), modelviewf, vtemp * 0.2850f + light, 1.60f);
		drawFlare(GLOW, Vec4f(0.90f, 0.90f, 0.20f, 0.5f * Alpha), modelviewf, vtemp * 0.2755f + light, 0.80f);
		drawFlare(GLOW, Vec4f(0.93f, 0.82f, 0.73f, 0.5f * Alpha), modelviewf, vtemp * 0.4775f + light, 1.00f);
		drawFlare(HALO, Vec4f(0.70f, 0.60f, 0.50f, 0.5f * Alpha), modelviewf, vtemp * 0.4900f + light, 1.40f);
		drawFlare(GLOW, Vec4f(0.70f, 0.80f, 0.30f, 0.5f * Alpha), modelviewf, vtemp * 0.6500f + light, 1.80f);
		drawFlare(GLOW, Vec4f(0.40f, 0.30f, 0.20f, 0.5f * Alpha), modelviewf, vtemp * 0.6300f + light, 1.40f);
		drawFlare(HALO, Vec4f(0.70f, 0.50f, 0.50f, 0.5f * Alpha), modelviewf, vtemp * 0.8000f + light, 1.40f);
		drawFlare(GLOW, Vec4f(0.80f, 0.50f, 0.10f, 0.5f * Alpha), modelviewf, vtemp * 0.7825f + light, 0.60f);
		drawFlare(HALO, Vec4f(0.50f, 0.50f, 0.70f, 0.5f * Alpha), modelviewf, vtemp * 1.0000f + light, 1.70f);
		drawFlare(GLOW, Vec4f(0.40f, 0.10f, 0.90f, 0.5f * Alpha), modelviewf, vtemp * 0.9750f + light, 2.00f);
		*/
		glEnd();
	}


	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);

	if (m_fadeTime > 1.0f)
		drawFlare(BIG_GLOW, Vec4f(1.0f, 1.0f, 0.0f, 1.0f / m_fadeTime), modelviewf, light, 32.0f);

	if (m_fadeTime < 1.0f) {
		drawFlare(BIG_GLOW, Vec4f(1.0f, 1.0f, 0.0f, 1.0f), modelviewf, light, 24.0f);
		drawFlare(BIG_GLOW, Vec4f(1.0f, 1.0f, 0.0f, 1.0f), modelviewf, light, 16.0f);
		if (m_fadeTime < 0.5f) {
			drawFlare(BIG_GLOW, Vec4f(1.0f, 1.0f, 0.0f, 10.0f), modelviewf, light, 16.0f);
			drawFlare(BIG_GLOW, Vec4f(1.0f, 1.0f, 0.0f, 1.0f), modelviewf, light, 12.0f);
			drawFlare(BIG_GLOW, Vec4f(1.0f, 1.0f, 0.0f, 1.0f), modelviewf, light, 8.0f);
		}
	}
	glEnd();
	glPopMatrix();
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
}

}
