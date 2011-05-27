/*
 * Simulation.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Simulation.hpp>

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
	  m_mouseAdapter(mouseAdapter)
{
	m_mouseAdapter.addListener(this);
}

Simulation::~Simulation()
{
	m_mouseAdapter.removeListener(this);
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
}


void Simulation::update()
{
	float delta = m_clock.get();
	m_clock.reset();

	float step = delta * (m_keyAdapter.shift() ? 125.0f : 75.0f);

	if (m_keyAdapter.isDown('w')) m_camera.move(step);
	if (m_keyAdapter.isDown('a')) m_camera.strafe(-step);
	if (m_keyAdapter.isDown('s')) m_camera.move(-step);
	if (m_keyAdapter.isDown('d')) m_camera.strafe(step);
}

void Simulation::render()
{
	m_camera.update();
	m_camera.apply();
}

}
