/**
 * @author Markus Doellinger
 * @date May 28, 2011
 * @file newton/util.hpp
 */

#ifndef NEWTON_UTIL_HPP_
#define NEWTON_UTIL_HPP_

#include <m3d/m3d.hpp>
#include <opengl/camera.hpp>
#include <Newton.h>
#include <list>

namespace newton {

using namespace m3d;

extern NewtonWorld* world;
extern float gravity;

/**
 * Calculates the forces resulting of the explosion at the given position
 * with the specified strength. Only bodies in the given radius are affected.
 *
 * @param position The position of the explosion
 * @param strength The strength of the explosion
 * @param radius   The radius of the explosion
 */
void applyExplosion(const Vec3f& position, float strength, float radius);

/**
 * Shoots a ray in world from origin in direction dir and returns
 * the first body that was hit. Returns NULL if no body was hit.
 *
 * @param origin The ray origin
 * @param dir    The ray direction
 * @return The first body hit with the ray, or NULL
 */
NewtonBody* getRayCastBody(const Vec3f& origin, const Vec3f& dir);


/**
 * Returns the vertical position of the world at the given position
 * of the plane. This function takes into consideration all collision
 * shapes and bodies of the given world.
 *
 * @param x     The x position in the plane
 * @param z     The z position in the plane
 * @return      The y position in the world at this position
 */
float getVerticalPosition(float x, float z);

/**
 * Does a convex cast of the body and returns the vertical position of
 * the body so that it just collides with the ground. This function takes
 * into consideration all collision shapes and bodies of the given world.
 * This also works with compound collisions.
 *
 * @param body The body to use for the convex cast
 * @return     The new vertical position of the body
 */
float getConvexCastPlacement(NewtonBody* body, std::list<NewtonBody*>* noCollision = NULL);

/**
 * Renders the specified collision shape, transformed with the given matrix.
 *
 * @param shape  The collision shape to render
 * @param matrix The transformation matrix
 */
void showCollisionShape(const NewtonCollision* shape, const Mat4f& matrix);

/**
 * Picks the object under the mouse coordinates and drags it the the world
 * position at the cursor.
 *
 * @param mouse The mouse coordinates
 * @param down  True, if the button is down, False otherwise
 * @return      True, if in pick-mode, False otherwise
 */
bool mousePick(const ogl::Camera& cam, const Vec2f& mouse, bool down);



}

#endif /* NEWTON_UTIL_HPP_ */
