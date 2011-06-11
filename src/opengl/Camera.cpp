/*
 * Camera.cpp
 *
 *  Created on: May 26, 2011
 *      Author: Markus Doellinger
 */

#include <opengl/Camera.hpp>
#include <GL/glew.h>

namespace ogl {


Camera::Camera()
{
	m_position = Vec3f(0.0f, 0.0f,  0.0f);
	m_eye 	   = Vec3f(0.0f, 0.0f, -1.0f);
	m_up       = Vec3f(0.0f, 1.0f,  0.0f);
	m_strafe   = Vec3f(0.0f, 0.0f,  0.0f);
	grabMatrix();
}

Camera::~Camera()
{
}

void Camera::grabMatrix()
{
	m_matrix = Mat4f::lookAt(m_position, m_eye, m_up);
	m_inverse = m_matrix.inverse();
}

void Camera::positionCamera(Vec3f position, Vec3f view, Vec3f up)
{
	m_position = position;
	m_eye      = position + view;
	m_up       = up;
	grabMatrix();
}

void Camera::move(float amount)
{
	Vec3f view = m_eye - m_position;
	view *= amount / view.len();

	//if (view.y < 0.0f && m_position.y <= 0.0f)
	//	view.y = 0.0f;

	m_position += view;
	m_eye += view;
	grabMatrix();
}

void Camera::strafe(float amount)
{
	m_position.x += m_strafe.x * amount;
	m_position.z += m_strafe.z * amount;

	m_eye.x += m_strafe.x * amount;
	m_eye.z += m_strafe.z * amount;
	grabMatrix();
}

void Camera::rotate(float angle, Vec3f axis)
{
	// rotation around the axis
	Quatf rot(axis, angle * 3.141f / 180.0f);

	// current rotation
	Quatf view(0.0f,
			   m_eye.x - m_position.x,
			   m_eye.y - m_position.y,
			   m_eye.z - m_position.z);

	// combine new and old rotation and reduce by old rotation
	Quatf updated = ((rot * view) * rot.conjugated());

	// add the change in rotation
	m_eye.x = m_position.x + updated.b;
	m_eye.y = m_position.y + updated.c;
	m_eye.z = m_position.z + updated.d;
	grabMatrix();
}

void Camera::update()
{
	m_strafe = (m_eye - m_position) % m_up;
	m_strafe.normalize();
}

void Camera::apply()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_position.x, m_position.y, m_position.z,
				   m_eye.x,	     m_eye.y,      m_eye.z,
				    m_up.x,       m_up.y,       m_up.z);
}

Vec3f Camera::viewVector() const
{
	return (m_eye - m_position).normalized();
};

Vec3f Camera::pointer() const
{
	Vec4<GLint> viewport = Vec4<GLint>::viewport();
	return pointer((viewport.z - viewport.x) / 2,
				   (viewport.w - viewport.y) / 2);
}

Vec3f Camera::pointer(int x, int y) const
{
	Mat4d modelview = Mat4d::modelview();
	Mat4d projection = Mat4d::projection();
	Vec4<GLint> viewport = Vec4<GLint>::viewport();
	GLfloat z;

	/// @todo move this from here
	y = viewport.w - y;

	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

	GLdouble _x, _y, _z;
	gluUnProject(x, y, z,
			modelview[0], projection[0], &viewport[0],
			&_x, &_y, &_z);

	return Vec3f(_x, _y, _z);
};

}
