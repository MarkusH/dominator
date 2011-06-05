/*
 * util.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: Markus Doellinger
 */

#include <opengl/util.hpp>
#include <GL/glew.h>

namespace ogl {


Vec3f screenToWorld(const Vec3d& screen)
{
	Vec4<GLint> view = Vec4<GLint>::viewport();
	Mat4d mv = Mat4d::modelview();
	Mat4d proj = Mat4d::projection();

	double x, y, z;
	y = view.w - screen.y;

	gluUnProject(screen.x, y, screen.z, mv[0], proj[0], &view[0], &x, &y, &z);
	return Vec3f(x, y, z);
}

Vec3f screenToWorld(const Vec2d& screen)
{
	Vec4<GLint> view = Vec4<GLint>::viewport();
	Mat4d mv = Mat4d::modelview();
	Mat4d proj = Mat4d::projection();

	double x, y, z;
	float screenZ;
	y = view.w - screen.y;

	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &screenZ);

	gluUnProject(screen.x, y, screenZ, mv[0], proj[0], &view[0], &x, &y, &z);
	return Vec3f(x, y, z);
}

}
