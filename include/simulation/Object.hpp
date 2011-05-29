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
	typedef enum { BOX, SPHERE } Type;

protected:
	Type m_type;

public:
	__Object(Type type);
	virtual ~__Object();

	virtual const Mat4f& getMatrix() = 0;
	virtual void setMatrix(const Mat4f& matrix) = 0;

	/**
	 * Checks whether this object contains the given NewtonBody.
	 *
	 * @param body The NewtonBody to find
	 * @return True, if the object contains the NewtonBody, false otherwise
	 */
	virtual bool contains(const NewtonBody* const body) = 0;

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

	static RigidBody createSphere(Mat4f matrix, float radius, float mass, const std::string& material = "");
	static RigidBody createBox(Mat4f matrix, float w, float h, float d, float mass, const std::string& material = "");

	friend class __RigidBody;
};

/**
 * A __Rigidbody is an object that is a single body.
 */
class __RigidBody : public __Object, public Body {
protected:
	std::string m_material;
public:
	__RigidBody(Type type, NewtonBody* body, const std::string& material = "");
	__RigidBody(Type type, const Mat4f& matrix, const std::string& material = "");
	__RigidBody(Type type, NewtonBody* body, const Mat4f& matrix, const std::string& material = "");

	virtual const Mat4f& getMatrix() { return Body::getMatrix(); }
	virtual void setMatrix(const Mat4f& matrix) { Body::setMatrix(matrix); }

	virtual bool contains(const NewtonBody* const body);

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	virtual void render();

	friend class __Object;
};

}

#endif /* OBJECT_HPP_ */
