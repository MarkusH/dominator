/**
 * @author Markus Holtermann
 * @date May 14, 2011
 * @file gui/renderwidget.cpp
 */

#include <gui/renderwidget.hpp>

#include <iostream>

#include <m3d/m3d.hpp>
#include <opengl/shader.hpp>
#include <opengl/texture.hpp>
#include <util/inputadapters.hpp>
#include <simulation/material.hpp>
#include <simulation/simulation.hpp>
#include <util/config.hpp>

#include <QtCore/QString>

#include <QtGui/QWheelEvent>
#include <QtGui/QWidget>

#include <QtOpenGL/QGLWidget>


using namespace m3d;

namespace gui {

RenderWidget::RenderWidget(QWidget* parent) :
	QGLWidget(parent)
{
	setFocusPolicy(Qt::WheelFocus);
	//updateGL();
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	//m_timer->start();
}

void RenderWidget::initializeGL()
{
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		util::ErrorAdapter::instance().displayErrorMessage("Could not initialize GLEW!");
		exit(1);
	}

	// load per-pixel lighting shader

	ogl::ShaderMgr::instance().load(util::Config::instance().get("enableShadows", false) ? "data/shaders_shadow/" : "data/shaders/");
	ogl::TextureMgr::instance().load("data/textures/");

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
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float) width / (float) height, 0.1f, 4096.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	sim::Simulation::instance().getCamera().m_viewport = Vec4<GLint>::viewport();
	sim::Simulation::instance().getCamera().m_projection = Mat4f::projection();
}

void RenderWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sim::Simulation::instance().update();
	sim::Simulation::instance().render();

	static int frames = 0;
	frames++;
	if (m_clock.get() >= 1.0f) {
		emit framesPerSecondChanged(frames);
		emit objectsCountChanged(sim::Simulation::instance().getObjectCount());
		if (sim::Simulation::instance().getSelectedObject()) {
			emit objectSelected(sim::Simulation::instance().getSelectedObject());
		}
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
		emit objectSelected(sim::Simulation::instance().getSelectedObject()->getType());
	} else {
		emit objectSelected();
	}
}

}
