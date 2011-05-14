/*
 * render/render.cpp
 *
 *      Author: Markus Holtermann
 */

#include "render/render.hpp"

#include <iostream>
#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>

Render::Render(QWidget *parent) :
	QGLWidget(parent) {
	//try {
		monkey = new Model3DS("data/cube.3ds");
		std::cout << "loaded" << std::endl;
	/*} catch (std::string error_str) {
		std::cerr << "Error: " << error_str << std::endl;
	}*/
}

void Render::initializeGL() {
	std::cout << "initializeGL" << std::endl;
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	// Enable lighting and set the position of the light
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	GLfloat pos[] = { 0.0, 4.0, 4.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	// Generate Vertex Buffer Objects
	monkey->CreateVBO();
}

void Render::resizeGL(int width, int height) {
	std::cout << "resizeGL" << std::endl;

	// Reset the viewport
	glViewport(0, 0, width, height);
	// Reset the projection and modelview matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// 10 x 10 x 10 viewing volume
	glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Render::paintGL() {
	std::cout << "paintGL" << std::endl;
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw our model
	monkey->Draw();

	// We don't need to swap the buffers, because QT does that automaticly for us
}
