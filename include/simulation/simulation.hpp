/*
 * Simulation.hpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#ifndef SIMULATION_HPP_
#define SIMULATION_HPP_

#include <util/inputadapters.hpp>
#include <util/clock.hpp>
#include <opengl/camera.hpp>
#include <opengl/vertexbuffer.hpp>
#include <opengl/skydome.hpp>
#include <simulation/object.hpp>
#include <map>
#include <Newton.h>
#include <iostream>


namespace sim {

using namespace m3d;

typedef std::list<Object> ObjectList;

/**
 * An object descriptor that can be used by the GUI to describe
 * the kind of object the user selected. The create() method will
 * return a new object of this type.
 */
struct ObjectInfo {
	__Object::Type type;
	std::string material;
	std::string fileName;
	float mass;
	int freezeState;

	/**
	 * BOX: x = width, y = height, z = depth
	 * SPHERE: x = radiusX, y = radiusY, z = radiusZ
	 * CYLINDER, CONE, CAPSULE, CHAMFER_CYLINDER x = radius, y = height
	 */
	Vec3f size;

	ObjectInfo(__Object::Type type, const std::string& material = "", const std::string& fileName = "",
			const float mass = -1.0f, const int freezeState = false, const Vec3f& size = Vec3f(1.0f, 1.0f, 1.0f));
	Object create(const Mat4f& matrix) const;
};

class Simulation : public util::MouseListener {
public:
	/**
	 * The interaction types.
	 */
	typedef enum {
		INT_NONE = 0,		/**< No interaction. */
		INT_ROTATE,			/**< Rotate object using the mouse. */
		INT_MOVE_GROUND,	/**< Move the object along the ground and perpendicular to the camera. */
		INT_MOVE_BILLBOARD,	/**< Move the object along the Y-axis and perpendicular to the camera. */
		INT_ROTATE_GROUND,  /**< Rotate the object around the Y-axis by moving the mouse along the ground plane */
		INT_DOMINO_CURVE,	/**< Create a domino curve by creating multiple control points */
		INT_CREATE_OBJECT
	} InteractionType;
private:
	static Simulation* s_instance;
	Simulation(util::KeyAdapter& keyAdapter, util::MouseAdapter& mouseAdapter);
	virtual ~Simulation();

protected:
	// input adapters
	util::KeyAdapter& m_keyAdapter;
	util::MouseAdapter& m_mouseAdapter;

	/** The interaction types for the buttons: util::LEFT util::RIGHT util::MIDDLE */
	InteractionType m_interactionTypes[3];

	util::Clock m_clock;

	ogl::Camera m_camera;

	/** The world position of the mouse pointer */
	Vec3f m_pointer;

	NewtonWorld* m_world;
	float m_gravity;
	bool m_enabled;
	__Object::Type m_newObjectType;
	std::string m_newObjectMaterial;
	std::string m_newObjectFilename;
	float m_newObjectMass;
	int m_newObjectFreezeState;
	Vec3f m_newObjectSize;

	int m_nextID;
	ObjectList m_objects;
	Object m_environment;

	/** The currently selected object, or an empty smart pointer */
	Object m_selectedObject;

	/**
	 * The vertex buffer contains the vertices, uvs and normals for all
	 * objects in the simulation. For each object, there are one or more
	 * sub-buffers that specify the indices of the related data. The first
	 * three sub-buffers are reserved for small, middle and large dominos.
	 * All other dominos reference this data in order to save memory on the
	 * GPU.
	 */
	ogl::VertexBuffer m_vertexBuffer;

	/**
	 * A list of sorted sub-buffers from the vertex buffer. We cannot sort
	 * the sub-buffers of the VBO directly because the order is important.
	 */
	ogl::SubBuffers m_sortedBuffers;

	/**
	 * The skydome of the simulation.
	 */
	ogl::Skydome m_skydome;
	Vec4f m_lightPos;

	/**
	 * Uploads the vertex data of all objects between begin
	 * and end, not including end.
	 *
	 * @param begin The first object to upload
	 * @param end   The end iterator
	 */
	void upload(const ObjectList::iterator& begin, const ObjectList::iterator& end);

	/**
	 * Checks if the given interaction type is activated in any button.
	 *
	 * @param type An interaction type
	 * @return     True, if enabled, false otherwise
	 */
	bool isActivated(InteractionType type);
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

	/** @param type Set the type of objects that will be created to type */
	void setNewObjectType(__Object::Type type);
	void setNewObjectMaterial(const std::string& material);
	void setNewObjectFilename(const std::string& filename);
	void setNewObjectMass(const float mass);
	void setNewObjectFreezeState(const int freezeState);
	void setNewObjectSize(const Vec3f size);

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
	InteractionType getInteractionType(util::Button button);

	/** @param type The new interaction type */
	void setInteractionType(util::Button button, InteractionType type);

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
	 * @param object The object to add
	 * @param id     The id of the object
	 * @return       The id of the object
	 */
	int add(const Object& object, int id);

	/**
	 * Adds a new object that is described by the given info struct.
	 *
	 * @param info The object descriptor
	 * @return     The id of the new object
	 */
	int add(const ObjectInfo& info);

	/**
	 * Removes the object from the simulation.
	 *
	 * @param object The object to remove
	 */
	void remove(const Object& object);

	/**
	 * Updates the geometry of the object by removing and re-adding it.
	 *
	 * @param object The object to update
	 */
	void updateObject(const Object& object);

	void save(const std::string& fileName);
	void load(const std::string& fileName);
	//void saveTemplate(const std::string& fileName, __Object& object);
	//void loadTemplate(const std::string& fileName);

	virtual void mouseMove(int x, int y);
	virtual void mouseButton(util::Button button, bool down, int x, int y);
	virtual void mouseDoubleClick(util::Button button, int x, int y);
	virtual void mouseWheel(int delta);


	void update();
	void render();
};


// inline methods

inline Simulation& Simulation::instance()
{
	return *s_instance;
}


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

inline void Simulation::setNewObjectType(__Object::Type type)
{
	m_newObjectType = type;
}

inline void Simulation::setNewObjectMaterial(const std::string& material)
{
	m_newObjectMaterial = material;
}

inline void Simulation::setNewObjectFilename(const std::string& filename)
{
	m_newObjectFilename = filename;
}

inline void Simulation::setNewObjectMass(const float mass)
{
	m_newObjectMass = mass;
}

inline void Simulation::setNewObjectFreezeState(const int freezeState)
{
	m_newObjectFreezeState = freezeState;
}

inline void Simulation::setNewObjectSize(const Vec3f size)
{
	m_newObjectSize = size;
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

inline Simulation::InteractionType Simulation::getInteractionType(util::Button button)
{
	return m_interactionTypes[button];
}

inline void Simulation::setInteractionType(util::Button button, InteractionType type)
{
	m_interactionTypes[button] = type;
}

inline bool Simulation::isActivated(InteractionType type)
{
	return m_interactionTypes[util::LEFT] == type ||
			m_interactionTypes[util::MIDDLE] == type ||
			m_interactionTypes[util::RIGHT] == type;
}

}

#endif /* SIMULATION_HPP_ */
