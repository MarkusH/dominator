/*
 * Body.cpp
 *
 *  Created on: May 28, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/body.hpp>
#include <newton/util.hpp>

namespace sim {

Body::Body()
	: m_matrix(Mat4f::identity()),
	  m_body(NULL)

{
}

Body::Body(NewtonBody* body)
	: m_matrix(Mat4f::identity()),
	  m_body(body)

{
}

Body::Body(const Mat4f& matrix)
	: m_matrix(matrix),
	  m_body(NULL)
{
}

Body::Body(NewtonBody* body, const Mat4f& matrix)
	: m_matrix(matrix),
	  m_body(body)

{
}

Body::~Body()
{
	if (m_body)
		NewtonDestroyBody(NewtonBodyGetWorld(m_body), m_body);
}

NewtonBody* Body::create(NewtonCollision* collision, float mass, int freezeState, const Vec4f& damping)
{
	Vec4f minBox, maxBox;
	Vec4f origin, inertia;

	m_body = NewtonCreateBody(newton::world, collision, this->m_matrix[0]);

	NewtonBodySetUserData(m_body, this);
	NewtonBodySetMatrix(m_body, this->m_matrix[0]);
	NewtonConvexCollisionCalculateInertialMatrix(collision, &inertia[0], &origin[0]);

	if (mass < 0.0f)
		mass = NewtonConvexCollisionCalculateVolume(collision) * 0.5f;

	if (mass != 0.0f)
		NewtonBodySetMassMatrix(m_body, mass, mass * inertia.x, mass * inertia.y, mass * inertia.z);

	NewtonBodySetCentreOfMass(m_body, &origin[0]);

	NewtonBodySetForceAndTorqueCallback(m_body, Body::__applyForceAndTorqueCallback);
	NewtonBodySetTransformCallback(m_body, Body::__setTransformCallback);
	NewtonBodySetDestructorCallback(m_body, Body::__destroyBodyCallback);

	NewtonBodySetFreezeState(m_body, freezeState);
	NewtonBodySetLinearDamping(m_body, damping.w);
	NewtonBodySetAngularDamping(m_body, &damping[0]);

	return m_body;
}

void Body::__destroyBodyCallback(const NewtonBody* body)
{
	//std::cout << "\tdestroy callback " << body << std::endl;
}

void Body::__setTransformCallback(const NewtonBody* body, const dFloat* matrix, int threadIndex)
{
	//std::cout << "\ttransform " << threadIndex << " " << body << std::endl;
	Body* _body = (Body*)NewtonBodyGetUserData(body);
	_body->m_matrix = Mat4f(matrix);
	//std::cout << "\ttransform end " << threadIndex << " " << body << std::endl;
}

void Body::__applyForceAndTorqueCallback(const NewtonBody* body, dFloat timestep, int threadIndex)
{
	//std::cout << "\tforce " << threadIndex << " " << body << std::endl;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;

	NewtonBodyGetMassMatrix(body, &mass, &Ixx, &Iyy, &Izz);
	Vec4f gravityForce(0.0f, mass * newton::gravity, 0.0f, 1.0f);
	NewtonBodySetForce(body, &gravityForce[0]);
	//std::cout << "\tforce end " << threadIndex << " " << body << std::endl;
}

}
