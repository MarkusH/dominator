/*
 * util.hpp
 *
 *  Created on: Jun 5, 2011
 *      Author: Markus Doellinger
 */

#ifndef OGL_UTIL_HPP_
#define OGL_UTIL_HPP_

#include <m3d/m3d.hpp>
using namespace m3d;

namespace ogl {


/**
 * Returns the world coordinates of the given position on the
 * viewport.
 *
 * @param screen The screen coordinates
 * @return       The world coordinates
 */
Vec3f screenToWorld(const Vec3d& screen);

/**
 * Returns the world coordinates of the given position on the
 * viewport. The z coordinate will be retrieved by glReadPixels.
 *
 * @param screen The screen coordinates
 * @return       The world coordinates
 */
Vec3f screenToWorld(const Vec2d& screen);

}

#endif /* OGL_UTIL_HPP_ */
