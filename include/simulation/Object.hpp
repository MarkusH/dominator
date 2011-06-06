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
#include <simulation/Body.hpp>
#include <opengl/VertexBuffer.hpp>
#include <lib3ds/file.h>

namespace sim {

class __Object;
typedef std::tr1::shared_ptr<__Object> Object;

class __RigidBody;
typedef std::tr1::shared_ptr<__RigidBody> RigidBody;

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
	typedef enum { BOX, SPHERE, CYLINDER, CAPSULE, CONE, CHAMFER_CYLINER, CONVEX_HULL, CONVEX_ASSEMBLY, COMPOUND, TREE_COLLISION } Type;

protected:
	Type m_type;
	int m_id;

public:
	__Object(Type type);
	virtual ~__Object();

	virtual const Mat4f& getMatrix() = 0;
	virtual void setMatrix(const Mat4f& matrix) = 0;

	int getID() const { return m_id; }
	void setID(int id) { m_id = id; }

	/** @param state The new freeze state of the body */
	virtual void setFreezeState(int state) { };

	/** @return The freeze state of the body */
	virtual int getFreezeState() { return 0; }

	/**
	 * Sets the vertical position of the object according to the convex cast of its collision.
	 *
	 * @param apply True, if the new position should be applied, False otherwise
	 * @return      The new vertical position
	 */
	virtual float convexCastPlacement(bool apply = true) = 0;

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
	virtual bool contains(const Object& object) = 0;

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
	static void save(const __Object& object /* node */);

	/**
	 * Loads an object from the given node.
	 *
	 * @return   The generated object
	 */
	static Object load(/* node */);

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
public:
	__RigidBody(Type type, NewtonBody* body, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	__RigidBody(Type type, const Mat4f& matrix, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	__RigidBody(Type type, NewtonBody* body, const Mat4f& matrix, const std::string& material = "", int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));

	virtual const Mat4f& getMatrix() { return Body::getMatrix(); }
	virtual void setMatrix(const Mat4f& matrix) { Body::setMatrix(matrix); }

	virtual void setFreezeState(int state) { m_freezeState = state; Body::setFreezeState(state); }
	virtual int getFreezeState() { return m_freezeState; }

	virtual float convexCastPlacement(bool apply = true);

	virtual bool contains(const NewtonBody* const body);
	virtual bool contains(const Object& object);

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	virtual void render();

	static void save(const __RigidBody& body /* node */);
	static RigidBody load(/*node */);

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
public:
	__ConvexHull(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
			bool originalGeometry = true, int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	~__ConvexHull();

	virtual void genBuffers(ogl::VertexBuffer& vbo);
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
public:
	__ConvexAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName, RenderingType renderingType = MESH_ASSEMBLY, int freezeState = 0, const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
	~__ConvexAssembly();

	virtual void genBuffers(ogl::VertexBuffer& vbo);
};



}

#endif /* OBJECT_HPP_ */
