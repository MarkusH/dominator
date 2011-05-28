/*
 * Simulation.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Simulation.hpp>
#include <opengl/Texture.hpp>
#include <iostream>
#include <newton/util.hpp>

namespace sim {

Simulation* Simulation::s_instance = NULL;

void Simulation::createInstance(util::KeyAdapter& keyAdapter,
								util::MouseAdapter& mouseAdapter)
{
	destroyInstance();
	s_instance = new Simulation(keyAdapter, mouseAdapter);
};

void Simulation::destroyInstance()
{
	if (s_instance) delete s_instance;
	s_instance = NULL;
};

Simulation& Simulation::instance()
{
	return *s_instance;
};

Simulation::Simulation(util::KeyAdapter& keyAdapter,
						util::MouseAdapter& mouseAdapter)
	: m_keyAdapter(keyAdapter),
	  m_mouseAdapter(mouseAdapter),
	  m_nextID(0)
{
	m_mouseAdapter.addListener(this);

	m_world = NewtonCreate();
}

Simulation::~Simulation()
{
	m_mouseAdapter.removeListener(this);
}




int Simulation::add(Object object)
{
	m_objects.insert(std::make_pair(++m_nextID, object));
	return m_nextID;
}

void Simulation::remove(Object object)
{
	ObjectMap::iterator itr = m_objects.begin();
	for ( ; itr != m_objects.end(); ++itr) {
		if (itr->second == object) {
			m_objects.erase(itr);
			break;
		}
	}
}



Object Simulation::selectObject(int x, int y)
{
	m_camera.apply();

	// Get the word coordinates at the viewport position, i.e.
	// the the 3D position the mouse pointer is
	Vec3f world = m_camera.pointer(x, y);
	Vec3f origin = m_camera.m_position.xyz();

	// Cast a ray from the camera position in the direction of the
	// selected world position
	NewtonBody* body = newton::getRayCastBody(m_world, origin, world - origin);

	// find the matching body
	ObjectMap::iterator itr = m_objects.begin();
	for ( ; body && itr != m_objects.end(); ++itr) {
		if (itr->second->contains(body))
			return itr->second;
	}

	// nothing was selected, return an empty smart pointer
	Object result;
	return result;
}


void Simulation::mouseMove(int x, int y)
{
	float angleX = (m_mouseAdapter.getX() - x) * 0.075f;
	float angleY = (m_mouseAdapter.getY() - y) * 0.1f;

	m_camera.rotate(angleX, Vec3f::yAxis());
	m_camera.rotate(angleY, m_camera.m_strafe.xyz());
}


void Simulation::mouseButton(util::Button button, bool down, int x, int y)
{
	if (!m_objects.size()) {
		Object obj = __Object::createSphere(Mat4f::identity(), 2.0f, 1.0f, "yellow");
		m_objects.insert(std::make_pair(0, obj));
	} else if (m_objects.size() == 1) {
		Object obj = __Object::createBox(Mat4f::identity(), 2.0f, 1.0f, 2.0f, 1.0f, "yellow");
		m_objects.insert(std::make_pair(1, obj));
	} else {
		Object selection = selectObject(x, y);
		std::cout << selection.get() << std::endl;
	}
}


void Simulation::mouseWheel(int delta) {
	float step = delta / 0.08;
	m_camera.move(step);
}

void Simulation::update()
{
	float delta = m_clock.get();
	m_clock.reset();

	float step = delta * (m_keyAdapter.shift() ? 2.5f : 1.25f);

	if (m_keyAdapter.isDown('w')) m_camera.move(step);
	if (m_keyAdapter.isDown('a')) m_camera.strafe(-step);
	if (m_keyAdapter.isDown('s')) m_camera.move(-step);
	if (m_keyAdapter.isDown('d')) m_camera.strafe(step);
}

void Simulation::render()
{
	m_camera.update();
	m_camera.apply();

	ogl::TextureMgr::instance().get("yellow")->bind();

	//glTranslatef(0.0f, 0.2f, -5.2f);
	ObjectMap::iterator itr = m_objects.begin();
	for ( ; itr != m_objects.end(); ++itr) {
		itr->second->render();
		/*
		glTranslatef(0.0f, 0.2f, -0.2f);
		GLUquadric* q = gluNewQuadric();
		gluQuadricTexture(q, GL_TRUE);
		gluQuadricNormals(q, GL_TRUE);
		gluSphere(q, 2.0f, 12, 24);
		gluDeleteQuadric(q);
		*/
	}
}

}
