/*
 * Object.hpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#ifndef OBJECT_HPP_
#define OBJECT_HPP_

#ifdef _WIN32
#include <boost/tr1/memory.hpp>
#else
#include <tr1/memory>
#endif
#include <string>
#include <simulation/body.hpp>
#include <opengl/vertexbuffer.hpp>
#include <lib3ds/file.h>
#include <xml/rapidxml.hpp>

namespace sim {

class __Object;
typedef std::tr1::shared_ptr<__Object> Object;

class __RigidBody;
typedef std::tr1::shared_ptr<__RigidBody> RigidBody;

class __ConvexAssembly;
typedef std::tr1::shared_ptr<__ConvexAssembly> ConvexAssembly;

class __ConvexHull;
typedef std::tr1::shared_ptr<__ConvexHull> ConvexHull;

/**
 * An __Object is an abstract class that represents all objects
 * in the simulation. It defines the type of the object and provides
 * several abstract methods for manipulating and rendering the objects.
 * The actual objects inherit from this class, and possibly "Body", to
 * form a final, usable object.
 *
 * The class also behaves as a factory to construct these specific objects.
 *
 * __Object should not be used directly, but as the smart pointer version
 * "Object".
 */
class __Object {
public:
	typedef enum {
		DOMINO_SMALL = 0,
		DOMINO_MIDDLE,
		DOMINO_LARGE,
		BOX,
		SPHERE,
		CYLINDER,
		CAPSULE,
		CONE,
		CHAMFER_CYLINDER,
		CONVEX_HULL,
		CONVEX_ASSEMBLY,
		COMPOUND,
		TREE_COLLISION,
		NONE
	} Type;
	static const char* TypeStr[];
	static const char* TypeName[];
	static const float TypeMass[];
	static const bool TypeFreezeState[];
	static const Vec3f TypeSize[];
protected:
	Type m_type;
	int m_id;

	__Object(Type type);
public:
	virtual ~__Object();

	const Type& getType() { return m_type; }

	virtual const Mat4f& getMatrix() const = 0;
	virtual void setMatrix(const Mat4f& matrix) = 0;

	int getID() const { return m_id; }
	void setID(int id) { m_id = id; }

	/** @param state The new freeze state of the body */
	virtual void setFreezeState(int state) { }

	/** @return The freeze state of the body */
	virtual int getFreezeState() { return 0; }

	/**
	 * Sets the mass of the object. -1.0 sets the mass according
	 * to the volume of the collision. 0.0 make the object static.
	 *
	 * @param mass The new mass of the object.
	 */
	virtual void setMass(float mass = -1.0f) { }

	/** @return The mass of the object */
	virtual float getMass() const { return 0.0f; }

	/** @return The size of the object, see scale() for further info */
	virtual Vec3f getSize() { return Vec3f(); }

	/** @param material Updates the material of the object */
	virtual void setMaterial(const std::string& material) { }
	virtual std::string getMaterial() { return ""; }

	/**
	 * Get the axis-aligned bounding box of the object.
	 *
	 * @param min The minimum
	 * @param max The maximum
	 */
	virtual void getAABB(Vec3f& min, Vec3f& max) = 0;

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
	virtual bool scale(const Vec3f& scale, bool add = false) { return false; };

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
	 * @return True, if the object contains the NewtonBody, false otherwise
	 */
	virtual bool contains(const NewtonBody* const body) = 0;

	/**
	 * Checks whether this object contains (or is) the given object.
	 *
	 * @param object
	 * @return
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

	/**
	 * Renders the object in debugging mode.
	 */
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
	 * @return   The generated object
	 */
	static Object load(rapidxml::xml_node<>* node);

	friend class __RigidBody;
};

/**
 * A __Rigidbody is an object that is a single body.
 */
class __RigidBody : public __Object, public Body {
protected:
	/** The material name of the body. It is a key in the map of the MaterialMgr */
	std::string m_material;

	/** defines if the body is freezed at creation time 0 = no, 1 = yes */
	int m_freezeState;

	/** The damping of the body. x,y,z = angular, w = linear damping */
	Vec4f m_damping;

	__RigidBody(Type type, NewtonBody* body, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	__RigidBody(Type type, const Mat4f& matrix, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	__RigidBody(Type type, NewtonBody* body, const Mat4f& matrix, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
public:
	virtual const Mat4f& getMatrix() const { return Body::getMatrix(); }
	virtual void setMatrix(const Mat4f& matrix) { Body::setMatrix(matrix); }

	virtual void setFreezeState(int state) { m_freezeState = state; Body::setFreezeState(state); }
	virtual int getFreezeState() { return m_freezeState; }

	virtual void setMass(float mass = -1.0f);
	virtual float getMass() const { return Body::getMass(); };
	virtual Vec3f getSize();

	virtual void setMaterial(const std::string& material);
	virtual std::string getMaterial() { return m_material; }

	virtual void getAABB(Vec3f& min, Vec3f& max) { NewtonBodyGetAABB(m_body, &min[0], &max[0]); }

	virtual bool scale(const Vec3f& scale, bool add = false);

	virtual float convexCastPlacement(bool apply = true, std::list<NewtonBody*>* noCollision = NULL);

	virtual bool contains(const NewtonBody* const body);
	virtual bool contains(const __Object* object);

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	virtual void render();

	static void save(const __RigidBody& body , rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);
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


	friend class __Object;
};

/**
 * A simple convex body with a single material. Generates a
 * convex hull of a 3ds file.
 */
class __ConvexHull : public __RigidBody {
protected:
	bool m_originalGeometry;
	NewtonMesh* m_mesh;
	std::vector<float> m_data;
	std::string m_fileName;

	__ConvexHull(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
			bool originalGeometry = true, int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
public:
	virtual ~__ConvexHull();

	static ConvexHull createHull(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
			bool originalGeometry = true, int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	static void save(const __ConvexHull& body , rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);
	static ConvexHull load(rapidxml::xml_node<>* node);
};

/**
 * A NewtonCompound is generated for each submesh of a 3ds file.
 */
class __ConvexAssembly : public __RigidBody  {
public:
	/**
	 * 1) a single mesh of the faces --> no smooth normals
	 * 2) a mesh for each convex hull --> convex submeshes
	 * 3) original 3ds data
	 */
	typedef enum { ORIGINAL = 0, MESH_EXACT, MESH_ASSEMBLY } RenderingType;
protected:
	RenderingType m_renderingType;
	NewtonMesh* m_mesh;
	std::vector<float> m_data;
	std::vector<uint32_t> m_indices;
	ogl::SubBuffers m_buffers;
	std::string m_fileName;

	__ConvexAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
			RenderingType renderingType = MESH_ASSEMBLY, int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
public:
	virtual ~__ConvexAssembly();

	static ConvexAssembly createAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
			RenderingType renderingType = MESH_ASSEMBLY, int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	static void save(const __ConvexAssembly& body , rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);
	static ConvexAssembly load(rapidxml::xml_node<>* node);
};



}

#endif /* OBJECT_HPP_ */
