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

namespace sim {

class __Object;
typedef std::tr1::shared_ptr<__Object> Object;

class __RigidBody;
typedef std::tr1::shared_ptr<__RigidBody> RigidBody;

class __Object {
public:
	typedef enum { BOX, SPHERE } Type;

protected:
	Type m_type;

public:
	__Object(Type type);
	virtual ~__Object();

	virtual void render() = 0;

	static RigidBody createSphere(Mat4f matrix, float radius, float mass, const std::string& material = "");

	friend class __RigidBody;
};

class __RigidBody : public __Object, public Body {
protected:
	std::string m_material;
public:
	__RigidBody(Type type, NewtonBody* body, const std::string& material = "");
	__RigidBody(Type type, const Mat4f& matrix, const std::string& material = "");
	__RigidBody(Type type, NewtonBody* body, const Mat4f& matrix, const std::string& material = "");

	virtual void render();

	friend class __Object;
};

}

#endif /* OBJECT_HPP_ */
