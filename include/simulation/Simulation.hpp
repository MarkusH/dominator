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

typedef std::list<Object> ObjectList;

class Simulation : public util::MouseListener {
public:
	typedef enum { INT_NONE = 0, INT_ROTATE, INT_MOVE_GROUND, INT_MOVE_BILLBOARD } InteractionType;
private:
	static Simulation* s_instance;
	Simulation(util::KeyAdapter& keyAdapter,
				util::MouseAdapter& mouseAdapter);
	virtual ~Simulation();

protected:
	// input adapters
	util::KeyAdapter& m_keyAdapter;
	util::MouseAdapter& m_mouseAdapter;

	InteractionType m_interactionType;

	util::Clock m_clock;

	ogl::Camera m_camera;

	/** The world position of the mouse pointer */
	Vec3f m_pointer;

	NewtonWorld* m_world;
	float m_gravity;
	bool m_enabled;

	int m_nextID;
	ObjectList m_objects;
	Object m_environment;

	/** The currently selected object, or an empty smart pointer */
	Object m_selectedObject;

	ogl::VertexBuffer m_vertexBuffer;

	/**
	 * Uploads the vertex data of all objects between begin
	 * and end, not including end.
	 *
	 * @param begin The first object to upload
	 * @param end   The end iterator
	 */
	void upload(const ObjectList::iterator& begin, const ObjectList::iterator& end);

	/**
	 * Applies the given material. If it is not available, disable all
	 * material properties.
	 *
	 * @param material
	 */
	void applyMaterial(const std::string& material);
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

	/**
	 * Initializes the Simulation. This has to be done before adding
	 * objects. Initializing the Simulation will clear the current
	 * state.
	 */
	void init();

	/**
	 * Clears the Simulation, including all objects in it.
	 */
	void clear();

	/** @param enabled True, if the simulation should be enabled, false otherwise */
	void setEnabled(bool enabled);

	/** @return True, if the simulation is enabled, false otherwise */
	bool isEnabled();

	/** @return The handle to the NewtonWorld */
	NewtonWorld* getWorld() const;

	/** @return Returns the current gravity */
	float getGravity() const;

	/** @param gravity The new gravity*/
	void setGravity(float gravity);

	/** @return The camera */
	ogl::Camera& getCamera();

	/** @return The number of objects in the simulation */
	unsigned getObjectCount();

	/** @return The current interaction type */
	InteractionType getInteractionType();

	/** @param type The new interaction type */
	void setInteractionType(InteractionType type);

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
	 * Returns the selected object.
	 *
	 * @return The selected object or an empty smart pointer.
	 */
	Object getSelectedObject();

	/**
	 * Adds the specified object to the simulation.
	 *
	 * @param object The object to add
	 * @return		 The id of the object
	 */
	int add(const Object& object);

	/**
	 * Adds the object and assigns the id to it
	 *
	 * @param
	 * @param id
	 * @return
	 */
	int add(const Object& object, int id);

	/**
	 * Removes the object from the simulation.
	 *
	 * @param object The object to remove
	 */
	void remove(Object object);

	void save(const std::string& fileName);
	void load(const std::string&fileName);


	virtual void mouseMove(int x, int y);
	virtual void mouseButton(util::Button button, bool down, int x, int y);
	virtual void mouseDoubleClick(util::Button button, int x, int y);
	virtual void mouseWheel(int delta);


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

inline void Simulation::setGravity(float gravity)
{
	m_gravity = gravity;
}

inline void Simulation::setEnabled(bool enabled)
{
	m_enabled = enabled;
}

inline bool Simulation::isEnabled()
{
	return m_enabled;
}

inline ogl::Camera& Simulation::getCamera()
{
	return m_camera;
}

inline unsigned Simulation::getObjectCount()
{
	return m_objects.size();
}

inline Object Simulation::getSelectedObject()
{
	return m_selectedObject;
}

inline Simulation::InteractionType Simulation::getInteractionType()
{
	return m_interactionType;
}

inline void Simulation::setInteractionType(InteractionType type)
{
	m_interactionType = type;
}

}

#endif /* SIMULATION_HPP_ */
