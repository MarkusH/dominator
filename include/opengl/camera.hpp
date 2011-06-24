/*
 * Camera.hpp
 *
 *  Created on: May 26, 2011
 *      Author: Markus Doellinger
 */

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <m3d/m3d.hpp>

namespace ogl {

using namespace m3d;

/**
 * A first person camera that supports forward and strafe
 * movement as well as rotations.
 */
class Camera {
public:
	typedef enum { OUTSIDE = 0, INTERSECT, INSIDE } Visibility;

	Camera();
	virtual ~Camera();

	/** The position of the camera */
	Vec3f m_position;

	/** The focus point of the camera, this is not the view vector! */
	Vec3f m_eye;

	/** The up vector of the camera, usually (0, 1, 0) */
	Vec3f m_up;

	/** A vector perpendicular to the viewVector() and the up vector */
	Vec3f m_strafe;

	Mat4f m_modelview;
	Mat4f m_projection;
	Mat4f m_inverse;
	Vec4<GLint> m_viewport;

	/**
	 * The six planes of the view frustum in the following order:
	 * left, right, bottom, top, near, far
	 */
	float m_frustum[6][4];

	/**
	 * Positions the camera at position,in direction view
	 * and the up-vector up.
	 *
	 * @param position The position of the camera
	 * @param view	   The view direction
	 * @param up	   The up-vector
	 */
	void positionCamera(Vec3f position, Vec3f view, Vec3f up);

	/**
	 * Moves the camera in the view direction.
	 *
	 * @param amount The amount to move.
	 */
	void move(float amount);

	/**
	 * Moves the camera perpendicular to the view direction.
	 *
	 * @param amount The amount to move.
	 */
	void strafe(float amount);

	/**
	 * Rotates the camera around the axis, using the specified
	 * angle.
	 *
	 * @param angle The angle to move
	 * @param axis  The rotation axis
	 */
	void rotate(float angle, Vec3f axis);

	/**
	 * Updates the strafe vector of the camera. Grabs the current
	 * matrix and calculates the inverse. It then updates the view frustum.
	 */
	void update();

	/**
	 * Applies the camera matrix.
	 */
	void apply();

	/**
	 * Returns the view vector of the camera, i.e the front.
	 *
	 * @return The view vector
	 */
	Vec3f viewVector() const;

	/**
	 * Returns the world-coordinates of the pixel in the middle
	 * of the screen. It is the same as shooting a ray from the
	 * camera position in view direction and returning the first
	 * contact point.
	 *
	 * @return The world-coordinates in the middle of the screen
	 */
	Vec3f pointer() const;

	/**
	 * Returns the world-coordinates of the pixel specified by
	 * x and y.
	 *
	 * @return The world-coordinates of the specified pixel
	 */
	Vec3f pointer(int x, int y) const;

	/**
	 * Checks whether the given axis-aligned bounding box is partially
	 * or fully visible. If so, returns True.
	 *
	 * @param min The minimum of the AABB
	 * @param max The maximum of the AABB
	 * @return    True, if the AABB is (partially) visible, False otherwise
	 */
	bool checkAABB(const Vec3f& min, const Vec3f& max) const;

	/**
	 * Tests the visibility of the given axis-aligned bounding box.
	 * Returns Visibility.OUTSIDE if the bounding box is completly
	 * outside of the frustum. Returns Visibility.INTERSECT if a part
	 * but not all of the AABB is inside the frustum. Returns
	 * Visibility.INSIDE if all of the AABB is inside the AABB.
	 *
	 * @param min The minimum of the AABB
	 * @param max The maximum of the AABB
	 * @return    The visibility of the AABB
	 */
	Visibility testAABB(const Vec3f& min, const Vec3f& max) const;

	/**
	 *
	 * @param center
	 * @param radius
	 * @return
	 */
	Visibility testSphere(const Vec3f& center, float radius) const;
};

}

#endif /* CAMERA_HPP_ */
