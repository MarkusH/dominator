/*
 * oglutil.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: Markus Doellinger
 */

#include <opengl/oglutil.hpp>
#include <GL/glew.h>

namespace ogl {

void drawAABB(const Vec3f& min, const Vec3f& max)
{
	glBegin(GL_LINES);

	glVertex3f(min.x, min.y, min.z);
	glVertex3f(min.x, min.y, max.z);

	glVertex3f(max.x, min.y, min.z);
	glVertex3f(max.x, min.y, max.z);

	glVertex3f(min.x, max.y, min.z);
	glVertex3f(min.x, max.y, max.z);

	glVertex3f(max.x, max.y, min.z);
	glVertex3f(max.x, max.y, max.z);

	glVertex3f(min.x, min.y, min.z);
	glVertex3f(max.x, min.y, min.z);

	glVertex3f(min.x, min.y, min.z);
	glVertex3f(min.x, max.y, min.z);

	glVertex3f(max.x, min.y, min.z);
	glVertex3f(max.x, max.y, min.z);

	glVertex3f(min.x, max.y, min.z);
	glVertex3f(max.x, max.y, min.z);

	glVertex3f(min.x, min.y, max.z);
	glVertex3f(max.x, min.y, max.z);

	glVertex3f(min.x, min.y, max.z);
	glVertex3f(min.x, max.y, max.z);

	glVertex3f(max.x, min.y, max.z);
	glVertex3f(max.x, max.y, max.z);

	glVertex3f(min.x, max.y, max.z);
	glVertex3f(max.x, max.y, max.z);
	glEnd();
}

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


Vec3f getScreenRay(const Vec2d& screen, Vec3f& near, Vec3f& far)
{
	Vec4<GLint> viewport = Vec4<GLint>::viewport();
	float _y = viewport.w - screen.y;
	Mat4d mvmatrix = Mat4d::modelview();
	Mat4d projmatrix = Mat4d::projection();

	// shoot a ray to the near and far plane
	double dX, dY, dZ;
	gluUnProject(screen.x, _y, 0.0, mvmatrix[0], projmatrix[0], &viewport[0], &dX, &dY, &dZ);
	near = Vec3f(dX, dY, dZ);
	gluUnProject(screen.x, _y, 1.0, mvmatrix[0], projmatrix[0], &viewport[0], &dX, &dY, &dZ);
	far = Vec3f(dX, dY, dZ);

	return (near - far).normalized();
}

}
