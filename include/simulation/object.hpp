/*
 * Object.hpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#ifndef OBJECT_HPP_
#define OBJECT_HPP_


#include <boost/tr1/memory.hpp>
#include <string>
#include <simulation/body.hpp>
#include <opengl/vertexbuffer.hpp>
#include <lib3ds/file.h>
#include <xml/rapidxml.hpp>
#include <opengl/mesh.hpp>

namespace sim {

class __Object;
typedef std::tr1::shared_ptr<__Object> Object;

class __RigidBody;
typedef std::tr1::shared_ptr<__RigidBody> RigidBody;

class __Convex;
typedef std::tr1::shared_ptr<__Convex> Convex;

/**
 * An abstract class that represents all objects in the simulation.
 * It defines the type of the object and provides several abstract
 * methods for manipulating and rendering the objects. The actual
 * objects inherit from this class, and possibly "Body", to form a
 * final, usable object.
 *
 * This class should not be used directly, but as the smart pointer
 * "Object".
 */
class __Object {
public:
	/** The different object types */
	typedef enum {
		DOMINO_SMALL = 0,//!< A small domino
		DOMINO_MIDDLE,   //!< A medium sized domino
		DOMINO_LARGE,    //!< A large domino
		BOX,             //!< A box with width, height, depth
		SPHERE,          //!< A sphere with different radiuses on all axes
		CYLINDER,        //!< A cylinder with radius and height
		CAPSULE,         //!< A capsule with radius and height
		CONE,            //!< A cone with radius and height
		CHAMFER_CYLINDER,//!< A chamfer cylinder (smooth edges) with radius and height
		CONVEX_HULL,     //!< A convex hull (physics) with a complex representation (graphics)
		CONVEX_ASSEMBLY, //!< A convex assembly (physics) with a complex representation (graphics)
		COMPOUND,        //!< A composition of objects and joints
		TREE_COLLISION,  //!< A complex, static collision for the environment
		NONE             //!< Dummy type
	} Type;

	/** Simple string representations for the types above */
	static const char* TypeStr[];

	/** Nice string representations for the types above */
	static const char* TypeName[];

	/** Default masses for the types above */
	static const float TypeMass[];

	/** Default freeze states for the types above */
	static const bool TypeFreezeState[];

	/** Default sizes for the types above */
	static const Vec3f TypeSize[];
protected:
	/** The type of the object */
	Type m_type;

	/** The (unique) id of the object */
	int m_id;

	/**
	 * Protected constructor to prevent direct public instantiation
	 *
	 * @param type The type of the object
	 */
	__Object(Type type);
public:
	virtual ~__Object();

	/** @return The type of the object */
	const Type& getType();

	/** @return The id of the object */
	int getID() const;

	/** @param id The new id of the object. Has to be unique */
	void setID(int id);

	/** @return The matrix of the object */
	virtual const Mat4f& getMatrix() const = 0;

	/** @param matrix The new matrix */
	virtual void setMatrix(const Mat4f& matrix) = 0;

	/** @param state The new freeze state of the body */
	virtual void setFreezeState(int state);

	/** @return The freeze state of the body */
	virtual int getFreezeState();

	/**
	 * Sets the mass of the object. -1.0 sets the mass according
	 * to the volume of the collision. 0.0 make the object static.
	 *
	 * @param mass The new mass of the object.
	 */
	virtual void setMass(float mass = -1.0f);

	/** @return The mass of the object */
	virtual float getMass() const;

	/** @param material Updates the material of the object */
	virtual void setMaterial(const std::string& material);

	/** @return The material of the object */
	virtual std::string getMaterial();

	/**
	 * Get the axis-aligned bounding box of the object.
	 *
	 * @param min The minimum
	 * @param max The maximum
	 */
	virtual void getAABB(Vec3f& min, Vec3f& max) = 0;

	/** @return The size of the object, see scale() for further information. */
	virtual Vec3f getSize();

	/**
	 * Scales the object. This does not have any effect on objects besides primitives.
	 * The scale parameter is used as follows:
	 *
	 * BOX:    x = width, y = height, z = depth
	 * SPHERE: x = radiusX, y = radiusY, z = radiusZ
	 * CYLINDER, CONE, CAPSULE, CHAMFER_CYLINDER x = radius, y = height
	 *
	 * @param scale The new size of the object
	 * @param add   If True, adds the values of scale to the current size
	 * @return      True, if the object was scaled, False otherwise
	 */
	virtual bool scale(const Vec3f& scale, bool add = false);

	/**
	 * Sets the vertical position of the object according to the convex cast of its collision.
	 *
	 * @param apply True, if the new position should be applied, False otherwise
	 * @return      The new vertical position
	 */
	virtual float convexCastPlacement(bool apply = true, std::list<NewtonBody*>* noCollision = NULL) = 0;

	/**
	 * Checks whether this object contains the given NewtonBody.
	 *
	 * @param body The NewtonBody to find
	 * @return     True, if the object contains the NewtonBody, false otherwise
	 */
	virtual bool contains(const NewtonBody* const body) = 0;

	/**
	 * Checks whether this object contains (or is) the given object.
	 *
	 * @param object The object to search for
	 * @return       True, if the object is contained
	 */
	virtual bool contains(const __Object* object) = 0;

	/**
	 * Generates the vertices, uv-coordinates, normals and indices (buffers)
	 * of this object. The data will be added to the given vertex buffer object.
	 *
	 * @param vbo The VBO the add the data to
	 * @return    The number of sub-buffers added
	 */
	virtual void genBuffers(ogl::VertexBuffer& vbo) = 0;

	/** Renders the object in debugging mode. */
	virtual void render() = 0;

	/**
	 * Saves the given object to the specified node by creating a
	 * new child node and appending it to the given one.
	 *
	 * @param object The object itself
	 */
	static void save(__Object& object, rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc);

	/**
	 * Loads an object from the given node.
	 *
	 * @return The generated object
	 */
	static Object load(rapidxml::xml_node<>* node);
};

/**
 * A rigid body in the simulation. It has an initial freeze state, a
 * damping vector and a material. By default, it provides factory methods
 * to create primitive objects. Other classes extend the functionality.
 */
class __RigidBody : public __Object, public Body {
protected:
	/** The material name of the body. It is a key in the map of the MaterialMgr */
	std::string m_material;

	/** defines if the body is freezed at creation time 0 = no, 1 = yes */
	int m_freezeState;

	/** The damping of the body. x,y,z = angular, w = linear damping */
	Vec4f m_damping;

	// protected constructors to prevent direct public instantiation
	__RigidBody(Type type, NewtonBody* body, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	__RigidBody(Type type, const Mat4f& matrix, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	__RigidBody(Type type, NewtonBody* body, const Mat4f& matrix, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
public:
	virtual const Mat4f& getMatrix() const;
	virtual void setMatrix(const Mat4f& matrix);

	virtual void setFreezeState(int state);
	virtual int getFreezeState();

	virtual void setMass(float mass = -1.0f);
	virtual float getMass() const;
	virtual Vec3f getSize();

	virtual void setMaterial(const std::string& material);
	virtual std::string getMaterial();

	virtual void getAABB(Vec3f& min, Vec3f& max);

	virtual bool scale(const Vec3f& scale, bool add = false);

	virtual float convexCastPlacement(bool apply = true, std::list<NewtonBody*>* noCollision = NULL);

	virtual bool contains(const NewtonBody* const body);
	virtual bool contains(const __Object* object);

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	virtual void render();

	/**
	 *	Saves RigidBody object to XML node and appends it to document
	 *
	 *	@param body		Object to save
	 *	@param parent	XML root node
	 */
	static void save(const __RigidBody& body , rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);

	/**
	 * Loads an object from the given node.
	 *
	 * @return The generated object
	 * @throws rapidxml::parse_error Attribute not found
	 */
	static RigidBody load(rapidxml::xml_node<>* node);

	static RigidBody createSphere(const Mat4f& matrix, float radius_x, float radius_y, float radius_z, float mass, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	static RigidBody createSphere(const Vec3f& position, float radius_x, float radius_y, float radius_z, float mass, const std::string& material = "");
	static RigidBody createSphere(const Mat4f& matrix, float radius, float mass, const std::string& material = "");
	static RigidBody createSphere(const Vec3f& position, float radius, float mass, const std::string& material);
	static RigidBody createBox(const Mat4f& matrix, float w, float h, float d, float mass, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	static RigidBody createBox(const Vec3f& position, float w, float h, float d, float mass, const std::string& material = "");
	static RigidBody createCylinder(const Mat4f& matrix, float radius, float height, float mass, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	static RigidBody createCylinder(const Vec3f& position, float radius, float height, float mass, const std::string& material = "");
	static RigidBody createChamferCylinder(const Mat4f& matrix, float radius, float height, float mass, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	static RigidBody createChamferCylinder(const Vec3f& position, float radius, float height, float mass, const std::string& material = "");
	static RigidBody createCapsule(const Mat4f& matrix, float radius, float height, float mass, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	static RigidBody createCapsule(const Vec3f& position, float radius, float height, float mass, const std::string& material = "");
	static RigidBody createCone(const Mat4f& matrix, float radius, float height, float mass, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	static RigidBody createCone(const Vec3f& position, float radius, float height, float mass, const std::string& material = "");
};

/**
 * A simple convex body with a single material. Generates a convex hull of
 * a 3ds file. The visual representation is the one defined in the model.
 *
 * A rigid body created from a model file. Each sub-mesh of the given model
 * is represented by a convex hull. By adding multiple meshes to the model,
 * this object can have a complex shape.
 */
class __Convex : public __RigidBody {
protected:
	/** The file the object was loaded from */
	std::string m_fileName;

	/** The visual representation of the object */
	ogl::Mesh m_visual;

	// protected constructor to prevent direct public instantiation
	__Convex(Type type, const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
			int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
public:
	virtual ~__Convex();

	/**
	 * Creates a new convex hull from the given model file.
	 *
	 * @param matrix      The matrix of the object
	 * @param mass        The mass of the object, where 0 = static and -1 = according to volume
	 * @param material    The material name of the object
	 * @param fileName    The model file
	 * @param freezeState The initial freeze state of the body, default = false
	 * @param damping     The initial damping vector of the body: x, y, z = angular, w = linear. default = 0.1 each
	 * @return            The new convex hull
	 */
	static Convex createHull(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
			int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));

	/**
	 * Creates a new convex assembly from the given model file.
	 *
	 * @param matrix      The matrix of the object
	 * @param mass        The mass of the object, where 0 = static and -1 = according to volume
	 * @param material    The material name of the object
	 * @param fileName    The model file
	 * @param freezeState The initial freeze state of the body, default = false
	 * @param damping     The initial damping vector of the body: x, y, z = angular, w = linear. default = 0.1 each
	 * @return            The new convex assembly
	 */
	static Convex createAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
			int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	/**
	 *	Saves Convex object to XML node and appends it to document
	 *
	 *	@param body		Object to save
	 *	@param parent	XML root node
	 */
	static void save(const __Convex& body, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);

	/**
	 * Loads an object from the given node.
	 *
	 * @return The generated object
	 * @throws rapidxml::parse_error Attribute not found
	 */
	static Convex load(rapidxml::xml_node<>* node);
};


inline
const __Object::Type& __Object::getType()
{
	return m_type;
}

inline
int __Object::getID() const
{
	return m_id;
}

inline
void __Object::setID(int id)
{
	m_id = id;
}

inline
void __Object::setFreezeState(int state)
{
}

inline
int __Object::getFreezeState()
{
	return 0;
}

inline
void __Object::setMass(float mass)
{
}

inline
float __Object::getMass() const
{
	return 0.0f;
}

inline
Vec3f __Object::getSize()
{ return Vec3f();
}

inline
void __Object::setMaterial(const std::string& material)
{
}

inline
std::string __Object::getMaterial()
{
	return "";
}

inline
bool __Object::scale(const Vec3f& scale, bool add)
{
	return false;
}



inline
const Mat4f& __RigidBody::getMatrix() const
{
	return Body::getMatrix();
}

inline
void __RigidBody::setMatrix(const Mat4f& matrix)
{
	Body::setMatrix(matrix);
}

inline
void __RigidBody::setFreezeState(int state)
{
	m_freezeState = state; Body::setFreezeState(state);
}

inline
int __RigidBody::getFreezeState()
{
	return m_freezeState;
}

inline
float __RigidBody::getMass() const
{
	return Body::getMass();
}

inline
std::string __RigidBody::getMaterial()
{
	return m_material;
}

inline
void __RigidBody::getAABB(Vec3f& min, Vec3f& max)
{
	NewtonBodyGetAABB(m_body, &min[0], &max[0]);
}


}

#endif /* OBJECT_HPP_ */
