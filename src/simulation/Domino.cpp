/*
 * Domino.cpp
 *
 *  Created on: Jun 7, 2011
 *      Author: markus
 */

#include <simulation/Domino.hpp>
#include <newton/util.hpp>
#include <simulation/Simulation.hpp>
#include <simulation/Material.hpp>

namespace sim {

#ifndef CONVEX_DOMINO
NewtonCollision* __Domino::s_domino_collision[3] = { NULL, NULL, NULL };
#endif
Vec3f __Domino::s_domino_size[3] = { Vec3f(3.0f, 8.0f, 0.5f) * 0.75f, Vec3f(3.0f, 8.0f, 0.5f), Vec3f(3.0f, 8.0f, 0.5f) * 1.25f };

#ifdef CONVEX_DOMINO
__Domino::__Domino(Type type, const Mat4f& matrix, const std::string& material)
	: __RigidBody(type, matrix, material, 0, Vec4f(0.4f, 0.4f, 0.4f, 0.4f))
#else
__Domino::__Domino(Type type, const Mat4f& matrix, const std::string& material)
	: __RigidBody(type, matrix, material, 1, Vec4f(0.1f, 0.1f, 0.1f, 0.1f))
#endif
{

}

__Domino::~__Domino()
{
}

#ifndef CONVEX_DOMINO
NewtonCollision*  __Domino::getCollision(Type type, int materialID)
{
	const NewtonWorld* world = Simulation::instance().getWorld();
	Mat4f identity = Mat4f::identity();
	Vec3f size = s_domino_size[type];

	return s_domino_collision[type] ? s_domino_collision[type] :
			(s_domino_collision[type] = NewtonCreateBox(world, size.x, size.y, size.z, materialID, identity[0]));
}
#endif

Domino __Domino::createDomino(Type type, const Mat4f& matrix, float mass, const std::string& material)
{
	const NewtonWorld* world = Simulation::instance().getWorld();
	const float VERTICAL_DELTA = 0.0001f;
	const int materialID = MaterialMgr::instance().getID(material);

	type = std::min(type, DOMINO_LARGE);
	Vec3f size = s_domino_size[type];
	Vec3f sz = size * 0.5f;

	Mat4f mat(matrix);
	Vec3f p0 = mat.getW();
	mat.setW(Vec3f());

	// generate points
	Vec3f p[4][2];
	p[0][0] = p[0][1] = p0 + Vec3f(sz.x, 0.0f, sz.z) * mat;
	p[0][0].y = newton::getVerticalPosition(world, p[0][0].x, p[0][0].z) + VERTICAL_DELTA;
	p[1][0] = p[1][1] = p0 + Vec3f(-sz.x, 0.0f, sz.z) * mat;
	p[1][0].y = newton::getVerticalPosition(world, p[1][0].x, p[1][0].z) + VERTICAL_DELTA;
	p[2][0] = p[2][1] = p0 + Vec3f(-sz.x, 0.0f, -sz.z) * mat;
	p[2][0].y = newton::getVerticalPosition(world, p[2][0].x, p[2][0].z) + VERTICAL_DELTA;
	p[3][0] = p[3][1] = p0 + Vec3f(sz.x, 0.0f, -sz.z) * mat;
	p[3][0].y = newton::getVerticalPosition(world, p[3][0].x, p[3][0].z) + VERTICAL_DELTA;

#ifdef CONVEX_DOMINO
	// calculate new vertPos
	float vertPos = (p[0][0].y + p[1][0].y + p[2][0].y + p[3][0].y) * 0.25f + size.y;
	p[0][1].y = p[1][1].y = p[2][1].y = p[3][1].y = vertPos;


	Vec3f com, intertia;
	Mat4f identity = Mat4f::identity();

	NewtonCollision* collision = NewtonCreateConvexHull(world, 8, &p[0][0][0], 3 * sizeof(float), 0.0f, materialID, identity[0]);
	NewtonConvexCollisionCalculateInertialMatrix(collision, &intertia[0], &com[0]);
	NewtonReleaseCollision(world, collision);

	// align vertices to center of mass
	for (int i = 0; i < 4; ++i) {
		p[i][0] -= com;
		p[i][1] -= com;
	}

	// create final collision
	collision = NewtonCreateConvexHull(world, 8, &p[0][0][0], 3 * sizeof(float), 0.0f, materialID, identity[0]);
	mat = Mat4f::translate(com);

	Domino result = Domino(new __Domino(type, mat, material));
	result->create(collision, mass, result->m_freezeState, result->m_damping);

	NewtonReleaseCollision(world, collision);
#else

	Mat4f identity = Mat4f::identity();
	mat = matrix;
	Vec3f pos = mat.getW();
	pos.y = std::max(std::max(std::max(p[0][0].y, p[1][0].y), p[2][0].y), p[3][0].y) + size.y * 0.5f;
	mat.setW(pos);

	NewtonCollision* collision = getCollision(type, materialID); //NewtonCreateBox(world, size.x, size.y, size.z, materialID, identity[0]);

	Domino result = Domino(new __Domino(type, mat, material));
	result->create(collision, mass, result->m_freezeState, result->m_damping);
	NewtonReleaseCollision(world, collision);
#endif

	return result;
}

}
