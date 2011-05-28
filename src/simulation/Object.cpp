/*
 * Object.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Object.hpp>
#include <simulation/Material.hpp>
#include <simulation/Simulation.hpp>
#include <newton/util.hpp>

namespace sim {

__Object::__Object(Type type)
	: m_type(type)
{
	// TODO Auto-generated constructor stub

}

__Object::~__Object()
{
	// TODO Auto-generated destructor stub
}

RigidBody __Object::createSphere(Mat4f matrix, float radius, float mass, const std::string& material)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::SPHERE, matrix, material));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateSphere(world, radius, radius, radius, materialID, identity[0]);

	result->create(collision, mass);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __Object::createBox(Mat4f matrix, float w, float h, float d, float mass, const std::string& material)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::BOX, matrix, material));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateBox(world, w, h, d, materialID, identity[0]);

	result->create(collision, mass);
	NewtonReleaseCollision(world, collision);

	return result;
}


__RigidBody::__RigidBody(Type type, NewtonBody* body, const std::string& material)
	: __Object(type), Body(body), m_material(material)

{
}

__RigidBody::__RigidBody(Type type, const Mat4f& matrix, const std::string& material)
	: __Object(type), Body(matrix), m_material(material)
{
}

__RigidBody::__RigidBody(Type type, NewtonBody* body, const Mat4f& matrix, const std::string& material)
	: __Object(type), Body(body, matrix), m_material(material)

{
}

bool __RigidBody::contains(const NewtonBody* const body)
{
	return m_body == body;
}


void __RigidBody::render()
{
	newton::showCollisionShape(getCollision(), m_matrix);
}




}
