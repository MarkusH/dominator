/*
 * render/render.cpp
 *
 *      Author: Markus Holtermann
 */

#include <gui/render.hpp>
#include <m3d/m3d.hpp>
#include <util/Clock.hpp>
#include <iostream>
#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QString>

using namespace m3d;

Render::Render(QString *filename, QWidget *parent) :
	QGLWidget(parent) {
	m_matrix = Mat4f::translate(Vec3f(0.0f, 0.0f, -5.0f));
	load(filename);
}

void Render::load(QString *filename) {
	try {
		model = new Model3DS(filename->toUtf8().constData());
		std::cout << "loaded " << filename->toUtf8().constData() << std::endl;
	} catch (std::string& error_str) {
		std::cerr << "Error: " << error_str << std::endl;
	}
}

void Render::initializeGL() {
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "could not initialize GLEW" << std::endl;
	}

	// load per-pixel lighting shader
	ogl::ShaderMgr::instance().load("data/shaders/");

	// set some material properties
	const float mat_diffuse[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	const float mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
	const float mat_shininess = 20.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, &mat_diffuse[0]);
	glMaterialfv(GL_FRONT, GL_AMBIENT, &mat_ambient[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &mat_specular[0]);
	glMaterialfv(GL_FRONT, GL_SHININESS, &mat_shininess);
	glEnable(GL_COLOR_MATERIAL);

	// set some light properties
	GLfloat ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat diffuse[4] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat specular[4] = { 0.5, 0.5, 0.5, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Generate Vertex Buffer Objects
	model->CreateVBO();
	m_clock.reset();
}

void Render::resizeGL(int width, int height) {
	// Reset the viewport
	glViewport(0, 0, width, height);
	// Reset the projection and modelview matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// 10 x 10 x 10 viewing volume
	//glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
	gluPerspective(60.0f, (float) width / (float) height, 0.1f, 1024.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Render::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ogl::ShaderMgr::instance().get("ppl")->bind();

	// set the position of the light
	static m3d::Vec4f lightPos(0.0f, 10.0f, 15.0f, 0.0f);
	glLightfv(GL_LIGHT0, GL_POSITION, &lightPos[0]);

	// multiply object matrix with current modelview matrix (i.e. the camera)
	glMultMatrixf(m_matrix[0]);

	//std::cout << m_matrix << std::endl;

	// Draw our model
	model->Draw();
	static int frames = 0;
	frames++;
	if (m_clock.get() >= 1.0f) {
		emit framesPerSecondChanged(frames);
		m_clock.reset();
		frames = 0;
	}
}

void Render::setRotationXInc(float x) {
	// straight forward, but complicated
	//	Vec3f pos = m_matrix.getW();
	//	m_matrix = Mat4f::rotX(x * PI / 180.0f) * m_matrix;
	//	m_matrix.setW(pos);


	//Mat4f m = Mat4f::rotX(x * PI / 180.0f).rotMultiply(m_matrix);
	m_matrix %= Mat4f::rotX(x * PI / 180.0f);
}

void Render::setRotationXDec(float x) {
	//	Vec3f pos = m_matrix.getW();
	//	m_matrix = Mat4f::rotX(-x * PI / 180.0f) * m_matrix;
	//	m_matrix.setW(pos);
	m_matrix %= Mat4f::rotX(x * PI / 180.0f);
}

void Render::setRotationYInc(float x) {
	//	Vec3f pos = m_matrix.getW();
	//	m_matrix = Mat4f::rotY(x * PI / 180.0f) * m_matrix;
	//	m_matrix.setW(pos);
	m_matrix %= Mat4f::rotY(x * PI / 180.0f);
}

void Render::setRotationYDec(float x) {
	//	Vec3f pos = m_matrix.getW();
	//	m_matrix = Mat4f::rotY(-x * PI / 180.0f) * m_matrix;
	//	m_matrix.setW(pos);
	m_matrix %= Mat4f::rotY(x * PI / 180.0f);
}

void Render::setRotationZInc(float x) {
	//	Vec3f pos = m_matrix.getW();
	//	m_matrix = Mat4f::rotZ(x * PI / 180.0f) * m_matrix;
	//	m_matrix.setW(pos);
	m_matrix %= Mat4f::rotZ(x * PI / 180.0f);
}

void Render::setRotationZDec(float x) {
	//	Vec3f pos = m_matrix.getW();
	//	m_matrix = Mat4f::rotZ(-x * PI / 180.0f) * m_matrix;
	//	m_matrix.setW(pos);
	m_matrix %= Mat4f::rotZ(x * PI / 180.0f);
}
