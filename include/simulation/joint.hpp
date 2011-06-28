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
#include <CustomBallAndSocket.h>
#include <CustomSlider.h>
#ifdef _WIN32
#include <boost/tr1/memory.hpp>
#else
#include <tr1/memory>
#endif
#include <list>

namespace sim {

using namespace m3d;

class __Object;
typedef std::tr1::shared_ptr<__Object> Object;
class __Joint;
typedef std::tr1::shared_ptr<__Joint> Joint;
class __Hinge;
typedef std::tr1::shared_ptr<__Hinge> Hinge;
class __Slider;
typedef std::tr1::shared_ptr<__Slider> Slider;
class __BallAndSocket;
typedef std::tr1::shared_ptr<__BallAndSocket> BallAndSocket;

/**
 * The base class for all joints. This class should not be instantiated.
 */
class __Joint {
public:
	typedef enum { HINGE, SLIDER, BALL_AND_SOCKET } Type;

	Type type;

	__Joint(Type type);

	/**
	 * This method is called whenever the matrix of the parent bodies changes.
	 * All sub-classes have to ensure that their pin and pivot points are updated
	 * according to the changes. The changes must not be forwarded to Newton but
	 * are required for XML serialization.
	 *
	 * @param inverse The old matrix, but inverted
	 * @param matrix  The new matrix
	 */
	virtual void updateMatrix(const Mat4f& inverse, const Mat4f& matrix) { };

	static void save(const __Joint& joint, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);
	static Joint load(const std::list<Object>& list, rapidxml::xml_node<>* node);
};

/**
 * A simple hinge at a pivot point. The given bodies can only move around
 * the given pin axis.
 */
class __Hinge : public __Joint, public CustomHinge {
protected:
	__Hinge(Vec3f pivot, Vec3f pinDir,
			const Object& child, const Object& parent,
			const dMatrix& pinAndPivot,
			const NewtonBody* childBody, const NewtonBody* parentBody,
			bool limited = false, float minAngle = -1.0f, float maxAngle = 1.0f);
public:
	Vec3f pivot;
	Vec3f pinDir;
	Object child;
	Object parent;
	bool limited;
	float minAngle;
	float maxAngle;

	virtual void updateMatrix(const Mat4f& inverse, const Mat4f& matrix);

	static Hinge create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited = false, float minAngle = -1.0f, float maxAngle = 1.0f);

	static void save(const __Hinge& hinge, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);
	static Hinge load(const std::list<Object>& list, rapidxml::xml_node<>* node);
};

/**
 * A joint where the two bodies can only move relative to each other
 * along a given axis.
 */
class __Slider : public __Joint, public CustomSlider {
protected:
	__Slider(Vec3f pivot, Vec3f pinDir,
			const Object& child, const Object& parent,
			const dMatrix& pinAndPivot,
			const NewtonBody* childBody, const NewtonBody* parentBody,
			bool limited = false, float minDist = -1.0f, float maxDist = 1.0f);
public:
	Vec3f pivot;
	Vec3f pinDir;
	Object child;
	Object parent;
	bool limited;
	float minDist;
	float maxDist;

	virtual void updateMatrix(const Mat4f& inverse, const Mat4f& matrix);

	static Slider create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited = false, float minDist = -1.0f, float maxDist = 1.0f);

	static void save(const __Slider& slider, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);
	static Slider load(const std::list<Object>& list, rapidxml::xml_node<>* node);
};

/**
 * A ball and socket joint at a pivot point. The cone and twist angles can be limited.
 */
class __BallAndSocket : public __Joint {
protected:
	CustomBallAndSocket* m_joint;

	__BallAndSocket(Vec3f pivot, Vec3f pinDir,
			const Object& child, const Object& parent,
			const dMatrix& pinAndPivot,
			const NewtonBody* childBody, const NewtonBody* parentBody,
			bool limited = false, float coneAngle = 0.0f, float minTwist = 0.0f, float maxTwist = 0.0f);
public:
	Vec3f pivot;
	Vec3f pinDir;
	Object child;
	Object parent;
	bool limited;
	// if limited:
	float coneAngle;
	float minTwist, maxTwist;

	~__BallAndSocket();

	virtual void updateMatrix(const Mat4f& inverse, const Mat4f& matrix);

	static BallAndSocket create(Vec3f pivot, Vec3f pinDir,
			const Object& child, const Object& parent,
			bool limited = false, float coneAngle = 0.0f, float minTwist = 0.0f, float maxTwist = 0.0f);

	static void save(const __BallAndSocket& ball, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);
	static BallAndSocket load(const std::list<Object>& list, rapidxml::xml_node<>* node);
};
}

#endif /* JOINT_HPP_ */
