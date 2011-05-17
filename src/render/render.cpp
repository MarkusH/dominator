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

	// Enable lighting and set the position of the light
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	GLfloat pos[] = { 0.0, 4.0, 4.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

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
