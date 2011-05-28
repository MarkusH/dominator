/*
 * Object.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Object.hpp>
#include <simulation/Material.hpp>
#include <simulation/Simulation.hpp>

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

static void DebugShowGeometryCollision(void* userData, int vertexCount, const dFloat* faceVertec, int id)
{
	int i;

	i = vertexCount - 1;
	Vec3f p0 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
	for (i = 0; i < vertexCount; i ++) {
		Vec3f p1 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
		glVertex3f(p0.x, p0.y, p0.z);
		glVertex3f(p1.x, p1.y, p1.z);
		p0 = p1;
	}
}

static void ShowCollisionShape(const NewtonCollision* shape, const Mat4f& matrix)
{
	glBegin(GL_LINES);
	NewtonCollisionForEachPolygonDo(shape, matrix[0], DebugShowGeometryCollision, NULL);
	glEnd();
}

void __RigidBody::render()
{
	ShowCollisionShape(getCollision(), m_matrix);
}




}
