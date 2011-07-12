/**
 * @author Markus Doellinger, Robert Waury
 * @date Jun 2, 2011
 * @file simulation/joint.hpp
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
#include <boost/tr1/memory.hpp>
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
	Vec3f pivot;
	Vec3f pinDir;
	Object child;
	Object parent;

	__Joint(Type type, Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent);

	/**
	 * This method is called whenever the matrix of the parent bodies changes.
	 * All sub-classes have to ensure that their pin and pivot points are updated
	 * according to the changes. The changes must not be forwarded to Newton but
	 * are required for XML serialization.
	 */
	virtual void updateMatrix() = 0;

	/**
	 * Saves Joint object to XML node and appends node to document
	 *
	 * @param	joint	Reference to Joint object to save
	 */
	static void save(const __Joint& joint, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);

	/**
	 * Loads Joint from XML node
	 *
	 * @param	list	List of already loaded Objects that are part of the Joint
	 * @param	node	Pointer to XML node
	 * @return	The generated Joint object
	 */
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
	bool limited;
	float minAngle;
	float maxAngle;

	static Hinge create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited = false, float minAngle = -1.0f, float maxAngle = 1.0f);

	virtual void updateMatrix();
	/**
	 * Saves Hinge object to XML node
	 *
	 * @param hinge		Reference to Hinge object to save
	 * @param parent	Pointer to root node
	 * @param doc		Pointer to XML document
	 */
	static void save(const __Hinge& hinge, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);

	/**
	 * Loads Joint from XML node
	 *
	 * @param	list	List of already loaded Objects that are part of the Hinge
	 * @param	node	Pointer to XML node
	 * @throw rapidxml::parse_error Attribute not found
	 * @return	The generated Hinge object
	 */
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
	bool limited;
	float minDist;
	float maxDist;

	static Slider create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited = false, float minDist = -1.0f, float maxDist = 1.0f);

	virtual void updateMatrix();
	/**
	 * Saves Slider object to XML node
	 *
	 * @param slider	Reference to Hinge object to save
	 * @param parent	Pointer to root node
	 * @param doc		Pointer to XML document
	 */
	static void save(const __Slider& slider, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);
	
	/**
	 * Loads Slider from XML node
	 *
	 * @param	list	List of already loaded Objects that are part of the Slider
	 * @param	node	Pointer to XML node
	 * @throw rapidxml::parse_error Attribute not found
	 * @return	The generated Slider object
	 */
	static Slider load(const std::list<Object>& list, rapidxml::xml_node<>* node);
};

/**
 * A ball and socket joint at a pivot point. The cone and twist angles can be limited.
 */
class __BallAndSocket : public __Joint {
protected:
	__BallAndSocket(Vec3f pivot, Vec3f pinDir,
			const Object& child, const Object& parent,
			const dMatrix& pinAndPivot,
			const NewtonBody* childBody, const NewtonBody* parentBody,
			bool limited = false, float coneAngle = 0.0f, float minTwist = 0.0f, float maxTwist = 0.0f);
public:
	bool limited;
	float coneAngle;
	float minTwist, maxTwist;

	~__BallAndSocket();

	static BallAndSocket create(Vec3f pivot, Vec3f pinDir,
			const Object& child, const Object& parent,
			bool limited = false, float coneAngle = 0.0f, float minTwist = 0.0f, float maxTwist = 0.0f);

	/**
	 * Saves BallAndSocket object to XML node
	 *
	 * @param ball		Reference to BallAndSocket object to save
	 * @param parent	Pointer to root node
	 * @param doc		Pointer to XML document
	 */
	static void save(const __BallAndSocket& ball, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);

	/**
	 * Loads BallAndSocket from XML node
	 *
	 * @param	list	List of already loaded Objects that are part of the BallAndSocket
	 * @param	node	Pointer to XML node
	 * @throw rapidxml::parse_error Attribute not found
	 * @return	The generated BallAndSocket object
	 */
	static BallAndSocket load(const std::list<Object>& list, rapidxml::xml_node<>* node);
};

class __BallAndSocketStd : public __BallAndSocket, public CustomBallAndSocket {
protected:
	__BallAndSocketStd(Vec3f pivot, Vec3f pinDir,
			const Object& child, const Object& parent,
			const dMatrix& pinAndPivot,
			const NewtonBody* childBody, const NewtonBody* parentBody,
			bool limited = false, float coneAngle = 0.0f, float minTwist = 0.0f, float maxTwist = 0.0f);

	virtual void updateMatrix();

	friend class __BallAndSocket;
};

class __BallAndSocketLimited : public __BallAndSocket, public CustomLimitBallAndSocket {
protected:
	__BallAndSocketLimited(Vec3f pivot, Vec3f pinDir,
			const Object& child, const Object& parent,
			const dMatrix& pinAndPivot,
			const NewtonBody* childBody, const NewtonBody* parentBody,
			bool limited = false, float coneAngle = 0.0f, float minTwist = 0.0f, float maxTwist = 0.0f);

	virtual void updateMatrix();

	friend class __BallAndSocket;
};

}

#endif /* JOINT_HPP_ */
