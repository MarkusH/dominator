/*
 * util.hpp
 *
 *  Created on: May 28, 2011
 *      Author: Markus Doellinger
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <m3d/m3d.hpp>
#include <Newton.h>

namespace newton {

using namespace m3d;

/**
 * Shoots a ray in world from origin in direction dir and returns
 * the first body that was hit. Returns NULL if no body was hit.
 *
 * @param world  The world to cast the ray in
 * @param origin The ray origin
 * @param dir    The ray direction
 * @return The first body hit with the ray, or NULL
 */
NewtonBody* getRayCastBody(const NewtonWorld* world, const Vec3f& origin, const Vec3f& dir);


/**
 * Returns the vertical position of the world at the given position
 * of the plane. This function takes into consideration all collision
 * shapes and bodies of the given world.
 *
 * @param world The world to get the position from
 * @param x     The x position in the plane
 * @param z     The z position in the plane
 * @return      The y position in the world at this position
 */
float getVerticalPosition(const NewtonWorld* world, float x, float z);

/**
 * Renders the specified collision shape, transformed with the given matrix.
 *
 * @param shape  The collision shape to render
 * @param matrix The transformation matrix
 */
void showCollisionShape(const NewtonCollision* shape, const Mat4f& matrix);


}

#endif /* UTIL_HPP_ */