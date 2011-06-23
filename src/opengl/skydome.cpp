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

Skydome::Skydome()
	: m_list(0),
	  m_radius(1000.0f * 0.01f)
{
	m_color = Vec4f(0.9f, 0.7f, 0.7f, 1.0f);
}

Skydome::Skydome(const std::string& texture, const std::string& shader, const std::string& fileName)
	: m_list(0),
	  m_radius(1000.0f * 0.01f)
{
	load(texture, shader, fileName);
}

Skydome::~Skydome() {
	clear();
}

void Skydome::load(const std::string& texture, const std::string& shader, const std::string& fileName)
{
	clear();
	m_shader = ShaderMgr::instance().get(shader);
	m_texture = TextureMgr::instance().get(texture);

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
				glVertex3fv(mesh->pointL[face->points[i]].pos);

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
	m_texture = Texture();
	m_time = 0.0f;
}

void Skydome::update(float dt)
{
	m_time += dt * 0.01f;
}

void Skydome::render()
{
	glEnable(GL_TEXTURE_2D);

	m_texture->bind();

	m_shader->bind();
	m_shader->setUniform1f("time", m_time);
	m_shader->setUniform1f("invHeight", 1.0f / 50.0f);
	m_shader->setUniform1f("invRadius", 2.0f / (10.0f * m_radius));
	m_shader->setUniform4fv("color", &m_color[0]);
	m_shader->setUniform1i("s_texture_0", 0);
	m_shader->setUniform1i("s_texture_1", 0);

	glPushMatrix();
	glDepthMask(GL_FALSE);
	glScalef(m_radius, m_radius, m_radius);
	glTranslatef(0.0f, -16.0f, 0.0f);
	glCallList(m_list);
	glDepthMask(GL_TRUE);
	glPopMatrix();
	__Shader::unbind();
}

}
