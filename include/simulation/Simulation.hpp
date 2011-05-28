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
#include <opengl/VertexBuffer.hpp>
#include <simulation/Object.hpp>
#include <map>
#include <Newton.h>


namespace sim {

using namespace m3d;

typedef std::map<int, Object> ObjectMap;

class Simulation : public util::MouseListener {
private:
	static Simulation* s_instance;
	Simulation(util::KeyAdapter& keyAdapter,
				util::MouseAdapter& mouseAdapter);
	virtual ~Simulation();

protected:
	// input adapters
	util::KeyAdapter& m_keyAdapter;
	util::MouseAdapter& m_mouseAdapter;

	util::Clock m_clock;

	ogl::Camera m_camera;

	NewtonWorld* m_world;
	float m_gravity;

	int m_nextID;
	ObjectMap m_objects;

	ogl::VertexBuffer m_vertexBuffer;

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



	/** @return The handle to the NewtonWorld */
	NewtonWorld* getWorld() const;

	/** @return Returns the current gravity */
	float getGravity() const;

	/** @return The camera */
	ogl::Camera& getCamera();


	/**
	 * Selects the object at the viewport position (x, y). If
	 * there is no object, returns an empty smart pointer.
	 *
	 * @param x The x coordinate of the viewport
	 * @param y The y coordinate of the viewport
	 * @return  The selected body, or an empty smart pointer
	 */
	Object selectObject(int x, int y);

	/**
	 * Adds the specified object to the simulation.
	 *
	 * @param object The object to add
	 * @return		 The id of the object
	 */
	int add(Object object);

	/**
	 * Removes the object from the simulation.
	 *
	 * @param object The object to remove
	 */
	void remove(Object object);


	virtual void mouseMove(int x, int y);
	virtual void mouseButton(util::Button button, bool down, int x, int y);


	void update();
	void render();
};


// inline methods

inline NewtonWorld* Simulation::getWorld() const
{
	return m_world;
}

inline float Simulation::getGravity() const
{
	return m_gravity;
}

inline ogl::Camera& Simulation::getCamera()
{
	return m_camera;
}

}

#endif /* SIMULATION_HPP_ */
