/*
 * Simulation.hpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#ifndef SIMULATION_HPP_
#define SIMULATION_HPP_

#include <util/InputAdapters.hpp>
#include <util/Clock.hpp>
#include <opengl/Camera.hpp>

namespace sim {

using namespace m3d;

class Simulation : public util::MouseListener {
private:
	static Simulation* s_instance;
	Simulation(util::KeyAdapter& keyAdapter,
				util::MouseAdapter& mouseAdapter);
	virtual ~Simulation();

protected:
	util::KeyAdapter& m_keyAdapter;
	util::MouseAdapter& m_mouseAdapter;
	util::Clock m_clock;

	ogl::Camera m_camera;

public:
	/**
	 * Creates a new instance of the Simulation.
	 *
	 * @param keyAdapter
	 * @param mouseAdapter
	 */
	static void createInstance(util::KeyAdapter& keyAdapter,
								util::MouseAdapter& mouseAdapter);

	/**
	 *
	 */
	static void destroyInstance();

	/**
	 * Returns the singleton instance of the Simulation.
	 *
	 * @return The Simulation.
	 */
	static Simulation& instance();

	virtual void mouseMove(int x, int y);
	virtual void mouseButton(util::Button button, bool down, int x, int y);


	void update();
	void render();
};

}

#endif /* SIMULATION_HPP_ */
