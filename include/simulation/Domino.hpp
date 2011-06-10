/*
 * Domino.hpp
 *
 *  Created on: Jun 7, 2011
 *      Author: markus
 */

#ifndef DOMINO_HPP_
#define DOMINO_HPP_

#include <simulation/Object.hpp>

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
 *  When moving a domino with a special angle adapted to the ground, the angle
 *  	will not be correct for other ground levels. We would have to create
 *  	a new one
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

class __Domino;
typedef std::tr1::shared_ptr<__Domino> Domino;


class __Domino : public __RigidBody {
protected:
#ifndef CONVEX_DOMINO
	static NewtonCollision* s_domino_collision[3];
	static NewtonCollision* getCollision(Type type, int materialID);
#endif
	static Vec3f s_domino_size[3];
public:
	__Domino(Type type, const Mat4f& matrix, const std::string& material = "");
	virtual ~__Domino();

#ifndef CONVEX_DOMINO
	virtual void genBuffers(ogl::VertexBuffer& vbo);

	static void genDominoBuffers(ogl::VertexBuffer& vbo);

	static void freeCollisions();
#endif
	static Domino createDomino(Type type, const Mat4f& matrix, float mass, const std::string& material = "");
};

}

#endif /* DOMINO_HPP_ */
