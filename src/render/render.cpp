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
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "could not initialize GLEW" << std::endl;
	}



    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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
	//glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
	gluPerspective(60.0f, (float)width / (float)height, 0.1f, 1024.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Render::paintGL() {
	std::cout << "paintGL" << std::endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glTranslatef(0.0f, 0.0f, -5.0f);
	// Draw our model
	monkey->Draw();

	// We don't need to swap the buffers, because QT does that automaticly for us
}
