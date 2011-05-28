/*
 * Body.hpp
 *
 *  Created on: May 28, 2011
 *      Author: Markus Doellinger
 */

#ifndef BODY_HPP_
#define BODY_HPP_

#include <Newton.h>
#include <m3d/m3d.hpp>

using namespace m3d;

namespace sim {

class Body {
private:
	static void __destroyBodyCallback(const NewtonBody* body);
	static void __setTransformCallback(const NewtonBody* body, const dFloat* matrix, int threadIndex);
	static void __applyForceAndTorqueCallback(const NewtonBody* body, dFloat timestep, int threadIndex);
protected:
	/** The matrix of the body, access has to be monitored
	 * in order to alert Newton when the matrix changes.
	 */
	Mat4f m_matrix;
public:
	Body();
	Body(NewtonBody* body);
	Body(const Mat4f& matrix);
	Body(NewtonBody* body, const Mat4f& matrix);
	virtual ~Body();

	/**
	 * Creates a new NewtonBody and assigns it to this object.
	 *
	 * @param collision The collision of this body
	 * @param mass		The mass of this body
	 * @return			The handle to the NewtonBody
	 */
	NewtonBody* create(NewtonCollision* collision, float mass);

	// TODO: lets see if we need member functions to propagate the events to child classes
	//virtual void destroyBodyCallback(const NewtonBody* body);
	//virtual void setTransformCallback(const NewtonBody* body, const dFloat* matrix, int threadIndex);
	//virtual void applyForceAndTorqueCallback(const NewtonBody* body, dFloat timestep, int threadIndex);

	/** @return The mass of this body */
	float getMass() const;

	/** @return The collision of this body */
	NewtonCollision* getCollision() const;

	/** @return The Matrix of this body */
	const Mat4f& getMatrix();

	/**
	 * Sets the matrix of this body.
	 *
	 * @param matrix The new matrix
	 */
	void setMatrix(const Mat4f& matrix);

	/**
	 * Sets the velocity of this body. Note that m_body has
	 * to be a valid handle.
	 *
	 * @param vel The new velocity of the body
	 */
	void setVelocity(Vec3f vel) const;

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

inline const Mat4f& Body::getMatrix()
{
	return m_matrix;
}

inline void Body::setMatrix(const Mat4f& matrix)
{
	m_matrix = matrix;
	NewtonBodySetMatrix(m_body, matrix[0]);
}

inline void Body::setVelocity(Vec3f vel) const
{
	NewtonBodySetVelocity(m_body, &vel[0]);
}

}

#endif /* BODY_HPP_ */
