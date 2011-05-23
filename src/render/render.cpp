/*
 * render/render.cpp
 *
 *      Author: Markus Holtermann
 */

#include <render/render.hpp>

#include <iostream>
#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QString>

Render::Render(QString *filename, QWidget *parent) :
	QGLWidget(parent) {
	rotate_x = 0.0f;
	rotate_y = 0.0f;
	rotate_z = 0.0f;
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
	m_shader = ogl::Shader::load("data/shaders/ppl.vs", "data/shaders/ppl.fs");
	m_shader->compile();

	// set the position of the light
	GLfloat pos[] = { 0.0, 10.0, 15.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

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
	GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat diffuse[4] = { 0.3, 0.3, 0.3, 1.0 };
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

	m_shader->bind();

	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(rotate_y, 0.0f, 1.0f, 0.0f);
	glRotatef(rotate_x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotate_z, 0.0f, 0.0f, 1.0f);

	// Draw our model
	model->Draw();
}

void Render::setRotationX(int value) {
	rotate_x = (float) value;
	updateGL();
}

void Render::setRotationY(int value) {
	rotate_y = (float) value;
	updateGL();
}

void Render::setRotationZ(int value) {
	rotate_z = (float) value;
	updateGL();
}
