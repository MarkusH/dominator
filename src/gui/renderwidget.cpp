/**
 * @author Markus Holtermann
 * @date May 14, 2011
 * @file gui/renderwidget.cpp
 */

#include <gui/renderwidget.hpp>
#include <m3d/m3d.hpp>
#include <util/clock.hpp>
#include <iostream>
#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QString>
#include <simulation/simulation.hpp>
#include <opengl/texture.hpp>
#include <iostream>
#include <simulation/material.hpp>

using namespace m3d;

namespace gui {

RenderWidget::RenderWidget(QWidget* parent) :
	QGLWidget(parent)
{
	setFocusPolicy(Qt::WheelFocus);
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	m_timer->start();
}

void RenderWidget::initializeGL()
{
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "could not initialize GLEW" << std::endl;
		exit(1);
	}

	// load per-pixel lighting shader
	ogl::ShaderMgr::instance().load("data/shaders/");
	ogl::TextureMgr::instance().load("data/textures/");

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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	sim::Simulation::createInstance(m_keyAdapter, m_mouseAdapter);
	sim::Simulation::instance().init();
	sim::Simulation::instance().setEnabled(false);

	m_clock.reset();
}

void RenderWidget::resizeGL(int width, int height)
{
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

void RenderWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	sim::Simulation::instance().update();
	sim::Simulation::instance().render();

	// set the position of the light
	static m3d::Vec4f lightPos(0.0f, 10.0f, 15.0f, 0.0f);
	glLightfv(GL_LIGHT0, GL_POSITION, &lightPos[0]);

	static int frames = 0;
	frames++;
	if (m_clock.get() >= 1.0f) {
		emit framesPerSecondChanged(frames);
		emit objectsCountChanged(sim::Simulation::instance().getObjectCount());
		m_clock.reset();
		frames = 0;
	}
}

void RenderWidget::keyPressEvent(QKeyEvent* event)
{
	m_keyAdapter.keyEvent(event);
}

void RenderWidget::keyReleaseEvent(QKeyEvent* event)
{
	m_keyAdapter.keyEvent(event);
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event)
{
	m_mouseAdapter.mouseEvent(event);
}

void RenderWidget::mousePressEvent(QMouseEvent* event)
{
	m_mouseAdapter.mouseEvent(event);
}

void RenderWidget::mouseReleaseEvent(QMouseEvent* event)
{
	m_mouseAdapter.mouseEvent(event);
}

void RenderWidget::wheelEvent(QWheelEvent* event)
{
	m_mouseAdapter.mouseWheelEvent(event);
}

void RenderWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	m_mouseAdapter.mouseEvent(event);

	if (sim::Simulation::instance().getSelectedObject()) {
		emit objectSelected(sim::Simulation::instance().getSelectedObject());
	} else {
		emit objectSelected();
	}
}

}
