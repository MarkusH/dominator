/*
 * Joint.cpp
 *
 *  Created on: Jun 2, 2011
 *      Author: markus
 */

#include <simulation/Joint.hpp>
#include <iostream>

namespace sim {

__Joint::__Joint(Type type)
	: type(type)
{

}


__Hinge::__Hinge(const dMatrix& pinAndPivot, const NewtonBody* child, const NewtonBody* parent)
	: __Joint(HINGE), CustomHinge(pinAndPivot, child, parent)
{

}

Hinge __Hinge::create(Vec3f pivot, Vec3f pinDir, const NewtonBody* child, const NewtonBody* parent)
{
	dMatrix pinAndPivot(GetIdentityMatrix());
	pinAndPivot.m_front = dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f);
	pinAndPivot.m_up = dVector(0.0f, 1.0f, 0.0f, 0.0f);
	pinAndPivot.m_right = pinAndPivot.m_front * pinAndPivot.m_up;
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);

	Hinge result = Hinge(new __Hinge(pinAndPivot, child, parent));
	return result;
}

}
