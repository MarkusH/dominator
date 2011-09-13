/**
 * @author Markus Doellinger
 * @date May 28, 2011
 * @file simulation/body.hpp
 */

#ifndef BODY_HPP_
#define BODY_HPP_

#include <Newton.h>
#include <m3d/m3d.hpp>

using namespace m3d;

namespace sim {

/**
 * This class is a wrapper for a NewtonBody. It provides methods
 * to create and modify a body. It also handles its transformation
 * and provides methods to query the state of the body, for example
 * to render it at its current position.
 */
class Body {
private:
	/**
	 * This callback is used to clear any resources of the body.
	 *
	 * @param body The NewtonBody handle
	 */
	static void __destroyBodyCallback(const NewtonBody* body);

	/**
	 * This callback is used to notify the body object of a transformation
	 * caused by the physics simulation.
	 *
	 * @param body        The NewtonBody handle
	 * @param matrix      The new matrix of the body after the transformation
	 * @param threadIndex The id of the calling thread
	 */
	static void __setTransformCallback(const NewtonBody* body, const dFloat* matrix, int threadIndex);

	/**
	 * This callback is used to update the force and torque of the body. It
	 * can be used to apply gravity and similar outer influences.
	 *
	 * @param body        The NewtonBody handle
	 * @param timestep    The time step of the update process
	 * @param threadIndex The id of the calling thread
	 */
	static void __applyForceAndTorqueCallback(const NewtonBody* body, dFloat timestep, int threadIndex);
protected:
	/** The matrix of the body, access has to be monitored
	 * in order to alert Newton when the matrix changes.
	 */
	Mat4f m_matrix;

public:
	/** Creates an empty body object. Does not create a NewtonBody. */
	Body();

	/**
	 * Creates a new body object for the given NewtonBody.
	 *
	 * @param body The NewtonBody to create the object for
	 */
	Body(NewtonBody* body);

	/**
	 * Creates an empty body object with the given matrix. Does not
	 * create a NewtonBody.
	 *
	 * @param matrix The matrix of the body object
	 */
	Body(const Mat4f& matrix);

	/**
	 * Creates a new body object for the given NewtonBody at the
	 * specified position.
	 *
	 * @param body   The NewtonBody to create the object for
	 * @param matrix The matrix of the body
	 */
	Body(NewtonBody* body, const Mat4f& matrix);

	/** Destroys the body object and the NewtonBody (if any). */
	virtual ~Body();

	/**
	 * Creates a new NewtonBody and assigns it to this object.
	 *
	 * @param collision   The collision of this body
	 * @param mass        The mass of this body
	 * @param freezeState 1, if the body should be frozen, 0 otherwise
	 * @param damping     The damping coefficients, x,y,z = angular, w = linear damping
	 * @return            The handle to the NewtonBody
	 */
	NewtonBody* create(NewtonCollision* collision, float mass, int freezeState = 0,
			const Vec4f& damping = Vec4f(0.1f, 0.1f, 0.1f, 0.1f));

	/** @return The mass of this body */
	float getMass() const;

	/** @return The collision of this body */
	NewtonCollision* getCollision() const;

	/** @return The Matrix of this body */
	virtual const Mat4f& getMatrix() const;

	/**
	 * Sets the matrix of this body.
	 *
	 * @param matrix The new matrix
	 */
	virtual void setMatrix(const Mat4f& matrix);

	/** @param state The new freeze state */
	virtual void setFreezeState(int state);

	/** @return The freeze state of the body */
	virtual int getFreezeState();

	/**
	 * Sets the velocity of this body. Note that m_body has
	 * to be a valid handle.
	 *
	 * @param vel The new velocity of the body
	 */
	void setVelocity(const Vec3f& vel) const;

	/** The body handle */
	NewtonBody* m_body;
};


inline float Body::getMass() const
{
	float m, ix, iy, iz;
	NewtonBodyGetMassMatrix(m_body, &m, &ix, &iy, &iz);
	return m;
}

inline NewtonCollision* Body::getCollision() const
{
	return NewtonBodyGetCollision(m_body);
}

inline const Mat4f& Body::getMatrix() const
{
	return m_matrix;
}

inline void Body::setMatrix(const Mat4f& matrix)
{
	m_matrix = matrix;
	NewtonBodySetMatrix(m_body, matrix[0]);
}

inline void Body::setVelocity(const Vec3f& vel) const
{
	NewtonBodySetVelocity(m_body, &vel[0]);
}

inline void Body::setFreezeState(int state)
{
	NewtonBodySetFreezeState(m_body, state);
}

inline int Body::getFreezeState()
{
	return NewtonBodyGetFreezeState(m_body);
}

}

#endif /* BODY_HPP_ */
