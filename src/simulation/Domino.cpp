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

/*
 * Use this to create a convex domino. It is a box shape, where the 4
 * bottom vertices are aligned to the ground using a RayCast.
 *
 * Pro:
 * 	It looks more credible, because the domino fits to the ground
 *  Even when not freezing the body at creation time, they remain standing on
 *  	ramps (they still fall, if the angle is too steep)
 *
 * Cons:
 * 	This is not realistic, all dominos are perfect boxes, normally
 *  There are many many domino shapes, so we cannot reuse the collisions and we
 *  	must add data to the VBO for many dominos (slower).
 *
 *
 * Comment the define to create dominos in box shapes. The box is not aligned
 * 	to the ground, but it is positioned, so that it does not intersect the ground.
 *
 * Pro:
 * 	We only need three collisions (small, middle, large) and three entries in
 * 		the VBO (superior performance)
 *  This is realistic, because dominos are boxes
 *
 * Cons:
 *   We have to freeze the domino at creation time, otherwise they fall even on
 *   	ramps with a flat angle
 *   When freezing them, they stand upright even on really steep ramps. This is
 *   	unrealistic and one can see that they stand on a single edge, the rest is
 *   	in the air. (we may unfreeze the body at creation time, if the difference
 *   	in the ground level at the 4 bottom vertices exceeds a threshold
 */
//#define CONVEX_DOMINO

namespace sim {

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

Domino __Domino::createDomino(const Type& type, const Mat4f& matrix, float mass, const std::string& material)
{
	const NewtonWorld* world = Simulation::instance().getWorld();
	const float VERTICAL_DELTA = 0.0001f;
	const int materialID = MaterialMgr::instance().getID(material);

	Vec3f size = Vec3f(3.0f, 8.0f, 0.5f);
	size *= (type == DOMINO_LARGE) ? 1.25f : (type == DOMINO_SMALL) ? 0.75f : 1.0f;
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
	NewtonCollision* collision = NewtonCreateBox(world, size.x, size.y, size.z, materialID, identity[0]);
	Domino result = Domino(new __Domino(type, mat, material));
	result->create(collision, mass, result->m_freezeState, result->m_damping);
	NewtonReleaseCollision(world, collision);
#endif

	return result;
}

}
