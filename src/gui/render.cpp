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
#include <simulation/Simulation.hpp>
#include <opengl/Texture.hpp>
#include <iostream>
#include <simulation/Material.hpp>

using namespace m3d;

Render::Render(QWidget *parent) :
	QGLWidget(parent)
{
	m_modified = true;
	m_matrix = Mat4f::translate(Vec3f(0.0f, 0.0f, -5.0f));
	setFocusPolicy(Qt::WheelFocus);
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	m_timer->start();
}

void Render::initializeGL()
{
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "could not initialize GLEW" << std::endl;
	}

	// load per-pixel lighting shader
	ogl::ShaderMgr::instance().load("data/shaders/");
	ogl::TextureMgr::instance().load("data/textures/");
	sim::MaterialMgr::instance().load("data/materials.xml");
	sim::MaterialMgr::instance().save("data/materials_test.xml");

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

	m_clock.reset();
}

void Render::resizeGL(int width, int height)
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

void Render::paintGL()
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
		emit
		objectsCountChanged(sim::Simulation::instance().getObjectCount());
		m_clock.reset();
		frames = 0;
	}
}

void Render::keyPressEvent(QKeyEvent *event)
{
	m_keyAdapter.keyEvent(event);
}

void Render::keyReleaseEvent(QKeyEvent *event)
{
	m_keyAdapter.keyEvent(event);
}

void Render::mouseMoveEvent(QMouseEvent *event)
{
	m_mouseAdapter.mouseEvent(event);
}

void Render::mousePressEvent(QMouseEvent *event)
{
	m_mouseAdapter.mouseEvent(event);
}

void Render::mouseReleaseEvent(QMouseEvent *event)
{
	m_mouseAdapter.mouseEvent(event);
}

void Render::wheelEvent(QWheelEvent *event)
{
	m_mouseAdapter.mouseWheelEvent(event);
}

void Render::mouseDoubleClickEvent(QMouseEvent* event)
{
	m_mouseAdapter.mouseEvent(event);

	if (sim::Simulation::instance().getSelectedObject()) {
		std::cout << sim::Simulation::instance().getSelectedObject()->getMatrix() << std::endl;
		emit objectSelected(true);
		std::cout << "get " << sim::Simulation::instance().getSelectedObject()->getMatrix().eulerAngles() << std::endl;
		emit objectSelected(&sim::Simulation::instance().getSelectedObject()->getMatrix());
	} else {
		emit objectSelected(false);
	}
}

void Render::save(const std::string& fileName)
{
	sim::Simulation::instance().save(fileName);
	m_modified = false;
}

void Render::open(const std::string& fileName)
{
	sim::Simulation::instance().load(fileName);
	m_modified = false;
}

void Render::renderSize(char axis, float size)
{
	std::cout << axis << " size " << size << std::endl;
	if (sim::Simulation::instance().getSelectedObject()) {
		m3d::Mat4f matrix = sim::Simulation::instance().getSelectedObject()->getMatrix();
		m3d::Mat4f helper = m3d::Mat4f::identity();
		switch (axis) {
		case 'x':
			helper._11 = size;
			break;
		case 'y':
			helper._22 = size;
			break;
		case 'z':
			helper._33 = size;
			break;
		}
		matrix = helper * matrix;
		sim::Simulation::instance().getSelectedObject()->setMatrix(matrix);
	}
}

void Render::renderLocation(char axis, float position)
{
	std::cout << axis << " position " << position << std::endl;
	if (sim::Simulation::instance().getSelectedObject()) {
		m3d::Mat4f matrix = sim::Simulation::instance().getSelectedObject()->getMatrix();
		switch (axis) {
		case 'x':
			matrix._41 = position;
			break;
		case 'y':
			matrix._42 = position;
			break;
		case 'z':
			matrix._43 = position;
			break;
		}
		sim::Simulation::instance().getSelectedObject()->setMatrix(matrix);
	}
}

void Render::renderRotation(float x, float y, float z)
{
	std::cout << " angle " << x << " " << y << " " << z << std::endl;
	if (sim::Simulation::instance().getSelectedObject()) {
		sim::Object obj = sim::Simulation::instance().getSelectedObject();

		Mat4f matrix = Mat4f::rotZ(z * PI / 180.0f) * Mat4f::rotX(x * PI / 180.0f) * Mat4f::rotY(y * PI / 180.0f) * Mat4f::translate(obj->getMatrix().getW());

		obj->setMatrix(matrix);
	}
}
