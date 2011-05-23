/*
 * m3d.hpp
 *
 * A 3D math package.
 *
 *  Created on: May 14, 2011
 *      Author: markus
 */

#ifndef M3D_HPP_
#define M3D_HPP_

#include <stdint.h>
#include <string>
#include <sstream>
#include <math.h>

#define M3D_USE_OPENGL 1

#ifdef M3D_USE_OPENGL
#include <GL/glew.h>
#endif

const double PI = 3.14159265358979323846;
const double EPSILON = 0.00001;

namespace m3d {

template<typename T> class Vec2;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int32_t> Vec2i;

template<typename T> class Vec3;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec3<int32_t> Vec3i;

template<typename T> class Vec4;
typedef Vec4<float> Vec4f;
typedef Vec4<double> Vec4d;
typedef Vec4<int32_t> Vec4i;

template<typename T> class Mat4;
typedef Mat4<float> Mat4f;
typedef Mat4<double> Mat4d;

template<typename T> class Quat;
typedef Quat<float> Quatf;
typedef Quat<double> Quatd;

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.hpp"
#include "quat.hpp"
#include "operators.hpp"

}

#endif /* M3D_HPP_ */
