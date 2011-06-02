/*
 * Joint.hpp
 *
 *  Created on: Jun 2, 2011
 *      Author: markus
 */

#ifndef JOINT_HPP_
#define JOINT_HPP_

#include <m3d/m3d.hpp>
#include <dMatrix.h>
#include <dVector.h>
#include <NewtonCustomJoint.h>
#include <CustomHinge.h>
#ifdef _WIN32
#include <boost/tr1/memory.hpp>
#else
#include <tr1/memory>
#endif

namespace sim {

using namespace m3d;

class __Object;
typedef std::tr1::shared_ptr<__Object> Object;
class __Joint;
typedef std::tr1::shared_ptr<__Joint> Joint;
class __Hinge;
typedef std::tr1::shared_ptr<__Hinge> Hinge;

class __Joint {
public:
	typedef enum { HINGE } Type;

	Type type;

	__Joint(Type type);
};

class __Hinge : public __Joint, public CustomHinge {
protected:
	__Hinge(const dMatrix& pinAndPivot, const NewtonBody* child, const NewtonBody* parent);
public:
	static Hinge create(Vec3f pivot, Vec3f pinDir, const NewtonBody* child, const NewtonBody* parent);
};


}

#endif /* JOINT_HPP_ */
