/*
 * Joint.cpp
 *
 *  Created on: Jun 2, 2011
 *      Author: markus
 */

#include <simulation/object.hpp>
#include <simulation/joint.hpp>
#include <iostream>
#include <util/tostring.hpp>


namespace sim {

__Joint::__Joint(Type type)
	: type(type)
{

}

void __Joint::save(const __Joint& joint, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc)
{
	switch (joint.type) {
	case HINGE:
		__Hinge::save((const __Hinge&)joint, parent, doc);
		return;
	case SLIDER:
		__Slider::save((const __Slider&)joint, parent, doc);
		return;
	case BALL_AND_SOCKET:
		__BallAndSocket::save((const __BallAndSocket&)joint, parent, doc);
		break;
	}
}

Joint __Joint::load(const std::list<Object>& list, rapidxml::xml_node<>* node)
{
	//type attribute
	if( std::string(node->first_attribute()->value()) == "hinge" ) return __Hinge::load(list, node);
	if( std::string(node->first_attribute()->value()) == "slider" ) return __Slider::load(list, node);
	if( std::string(node->first_attribute()->value()) == "ballandsocket" ) return __BallAndSocket::load(list, node);

	Joint result;
	return result;
}

__Hinge::__Hinge(Vec3f pivot, Vec3f pinDir,
		const Object& child, const Object& parent,
		const dMatrix& pinAndPivot,
		const NewtonBody* childBody, const NewtonBody* parentBody,
		bool limited, float minAngle, float maxAngle)
	: __Joint(HINGE), CustomHinge(pinAndPivot, childBody, parentBody),
	  pivot(pivot), pinDir(pinDir),
	  child(child), parent(parent),
	  limited(limited), minAngle(minAngle), maxAngle(maxAngle)
{
	this->EnableLimits(limited);
	this->SetLimis(minAngle, maxAngle);
}

void __Hinge::updateMatrix(const Mat4f& inverse, const Mat4f& matrix)
{
	Mat4f tmp = inverse;
	tmp.setW(Vec3f());
	pinDir *= tmp;
	pinDir = pinDir % matrix;

	pivot *= inverse * matrix;
}

Hinge __Hinge::create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited, float minAngle, float maxAngle)
{
	__RigidBody* childBody = dynamic_cast<__RigidBody*>(child.get());
	__RigidBody* parentBody = parent ? dynamic_cast<__RigidBody*>(parent.get()) : NULL;

	dMatrix pinAndPivot(GetIdentityMatrix());
	pinAndPivot.m_front = dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f);
	pinAndPivot.m_up = dVector(0.0f, 1.0f, 0.0f, 0.0f);
	pinAndPivot.m_right = pinAndPivot.m_front * pinAndPivot.m_up;
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);

	Hinge result = Hinge(new __Hinge(pivot, pinDir, child, parent, pinAndPivot, childBody->m_body, parentBody ? parentBody->m_body : NULL, limited, minAngle, maxAngle));
	return result;
}

void __Hinge::save(const __Hinge& hinge, rapidxml::xml_node<>* sibling, rapidxml::xml_document<>* doc)
{
	using namespace rapidxml;

	xml_node<>* node = doc->allocate_node(node_element, "joint");
	sibling->append_node(node);
	
	// type attribute
	char* pType = doc->allocate_string("hinge");
	xml_attribute<>* attrT = doc->allocate_attribute("type", pType);
	node->append_attribute(attrT);
		
	// parentID attribute
	int parentID = hinge.parent ? hinge.parent->getID() : -1;
	char* pParentID = doc->allocate_string(util::toString(parentID));
	xml_attribute<>* attrP = doc->allocate_attribute("parentID", pParentID);
	node->append_attribute(attrP);
	
	// childID attribute
	int childID = hinge.child->getID(); // error occurs at getID() -33686019
	char* pChildID = doc->allocate_string(util::toString(childID));
	xml_attribute<>* attrC = doc->allocate_attribute("childID", pChildID);
	node->append_attribute(attrC);
	
	// pivot attribute
	char* pPivot = doc->allocate_string(hinge.pivot.str().c_str()); // hinge.pivot contains bullshit
	xml_attribute<>* attrPi = doc->allocate_attribute("pivot", pPivot);
	node->append_attribute(attrPi);

	// pinDir attribute
	char* pPinDir = doc->allocate_string(hinge.pinDir.str().c_str()); // hinge.pinDir doesn't contain the value at load
	xml_attribute<>* attrPD = doc->allocate_attribute("pinDir", pPinDir);
	node->append_attribute(attrPD);

	// limited attribute
	char* pLimited = doc->allocate_string(util::toString(hinge.limited));
	xml_attribute<>* attrLi = doc->allocate_attribute("limited", pLimited);
	node->append_attribute(attrLi);

	if (hinge.limited) {
		// minDist attribute
		char* pMinAngle = doc->allocate_string(util::toString(hinge.minAngle));
		xml_attribute<>* attrMi = doc->allocate_attribute("minangle", pMinAngle);
		node->append_attribute(attrMi);

		// maxDist attribute
		char* pMaxAngle = doc->allocate_string(util::toString(hinge.maxAngle));
		xml_attribute<>* attrMa = doc->allocate_attribute("maxangle", pMaxAngle);
		node->append_attribute(attrMa);
	}
}

Hinge __Hinge::load(const std::list<Object>& list, rapidxml::xml_node<>* node)
{
	using namespace rapidxml;

	Vec3f pivot, pinDir;
	int parentID = -1, childID = -1;

	//type attribute
	xml_attribute<>* attr = node->first_attribute();

	//parentID attribute
	attr = attr->next_attribute("parentID");
	parentID = atoi(attr->value());

	//childID attribute
	attr = attr->next_attribute("childID");
	childID = atoi(attr->value());

	//pivot attribute
	attr = attr->next_attribute("pivot");
	pivot.assign(attr->value());

	//pinDir attribute
	attr = attr->next_attribute("pinDir");
	pinDir.assign(attr->value());

	// Get the objects with the required IDs out of the object list
	Object parent, child;
	for (std::list<Object>::const_iterator itr = list.begin(); itr != list.end(); ++itr) {
		if ((*itr)->getID() == parentID)
			parent = *itr;
		if ((*itr)->getID() == childID)
			child = *itr;
	}

	// limited attribute
	attr = attr->next_attribute("limited");
	bool limited = atoi(attr->value());
	float minAngle = 0.0f, maxAngle = 0.0f;

	if (limited) {
		attr = attr->next_attribute("minangle");
		minAngle = atof(attr->value());

		attr = attr->next_attribute("maxangle");
		maxAngle = atof(attr->value());
	}


	Hinge hinge = __Hinge::create(pivot, pinDir, child, parent, limited, minAngle, maxAngle);
	return hinge;
}


__Slider::__Slider(Vec3f pivot, Vec3f pinDir,
		const Object& child, const Object& parent,
		const dMatrix& pinAndPivot,
		const NewtonBody* childBody, const NewtonBody* parentBody,
		bool limited, float minDist, float maxDist)
	: __Joint(SLIDER), CustomSlider(pinAndPivot, childBody, parentBody),
	  pivot(pivot), pinDir(pinDir),
	  child(child), parent(parent),
	  limited(limited), minDist(minDist), maxDist(maxDist)
{
	this->EnableLimits(limited);
	this->SetLimis(minDist, maxDist);
}

void __Slider::updateMatrix(const Mat4f& inverse, const Mat4f& matrix)
{
	Mat4f tmp = inverse;
	tmp.setW(Vec3f());
	pinDir *= tmp;
	pinDir = pinDir % matrix;

	pivot *= inverse * matrix;
}

Slider __Slider::create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited, float minDist, float maxDist)
{
	__RigidBody* childBody = dynamic_cast<__RigidBody*>(child.get());
	__RigidBody* parentBody = parent ? dynamic_cast<__RigidBody*>(parent.get()) : NULL;

	dMatrix pinAndPivot(GetIdentityMatrix());
	pinAndPivot.m_front = dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f);
	pinAndPivot.m_up = dVector(0.0f, 1.0f, 0.0f, 0.0f);
	pinAndPivot.m_right = pinAndPivot.m_front * pinAndPivot.m_up;
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);

	Slider result = Slider(new __Slider(pivot, pinDir, child, parent,
			pinAndPivot, childBody->m_body, parentBody ? parentBody->m_body : NULL, limited, minDist, maxDist));
	return result;
}

void __Slider::save(const __Slider& slider, rapidxml::xml_node<>* sibling, rapidxml::xml_document<>* doc)
{
	using namespace rapidxml;

	xml_node<>* node = doc->allocate_node(node_element, "joint");
	sibling->append_node(node);

	// type attribute
	char* pType = doc->allocate_string("slider");
	xml_attribute<>* attrT = doc->allocate_attribute("type", pType);
	node->append_attribute(attrT);

	// parentID attribute
	int parentID = slider.parent ? slider.parent->getID() : -1;
	char* pParentID = doc->allocate_string(util::toString(parentID));
	xml_attribute<>* attrP = doc->allocate_attribute("parentID", pParentID);
	node->append_attribute(attrP);

	// childID attribute
	int childID = slider.child->getID();
	char* pChildID = doc->allocate_string(util::toString(childID));
	xml_attribute<>* attrC = doc->allocate_attribute("childID", pChildID);
	node->append_attribute(attrC);

	// pivot attribute
	char* pPivot = doc->allocate_string(slider.pivot.str().c_str());
	xml_attribute<>* attrPi = doc->allocate_attribute("pivot", pPivot);
	node->append_attribute(attrPi);

	// pinDir attribute
	char* pPinDir = doc->allocate_string(slider.pinDir.str().c_str());
	xml_attribute<>* attrPD = doc->allocate_attribute("pinDir", pPinDir);
	node->append_attribute(attrPD);

	// limited attribute
	char* pLimited = doc->allocate_string(util::toString(slider.limited));
	xml_attribute<>* attrLi = doc->allocate_attribute("limited", pLimited);
	node->append_attribute(attrLi);

	if (slider.limited) {
		// minDist attribute
		char* pMinDist = doc->allocate_string(util::toString(slider.minDist));
		xml_attribute<>* attrMi = doc->allocate_attribute("mindist", pMinDist);
		node->append_attribute(attrMi);

		// maxDist attribute
		char* pMaxDist = doc->allocate_string(util::toString(slider.maxDist));
		xml_attribute<>* attrMa = doc->allocate_attribute("maxdist", pMaxDist);
		node->append_attribute(attrMa);
	}

}

Slider __Slider::load(const std::list<Object>& list, rapidxml::xml_node<>* node)
{
	using namespace rapidxml;

	Vec3f pivot, pinDir;
	int parentID = -1, childID = -1;

	//type attribute
	xml_attribute<>* attr = node->first_attribute();

	//parentID attribute
	attr = attr->next_attribute();
	parentID = atoi(attr->value());

	//childID attribute
	attr = attr->next_attribute();
	childID = atoi(attr->value());

	//pivot attribute
	attr = attr->next_attribute();
	pivot.assign(attr->value());

	//pinDir attribute
	attr = attr->next_attribute();
	pinDir.assign(attr->value());


	// Get the objects with the required IDs out of the object list
	Object parent, child;
	for (std::list<Object>::const_iterator itr = list.begin(); itr != list.end(); ++itr) {
		if ((*itr)->getID() == parentID)
			parent = *itr;
		if ((*itr)->getID() == childID)
			child = *itr;
	}

	// limited attribute
	attr = attr->next_attribute();
	bool limited = atoi(attr->value());
	float minDist = 0.0f, maxDist = 0.0f;

	if (limited) {
		attr = attr->next_attribute();
		minDist = atof(attr->value());

		attr = attr->next_attribute();
		maxDist = atof(attr->value());
	}

	Slider slider = __Slider::create(pivot, pinDir, child, parent, limited, minDist, maxDist);
	return slider;
}


__BallAndSocket::__BallAndSocket(Vec3f pivot, Vec3f pinDir,
		const Object& child, const Object& parent,
		const dMatrix& pinAndPivot,
		const NewtonBody* childBody, const NewtonBody* parentBody,
		bool limited, float coneAngle, float minTwist, float maxTwist)
	: __Joint(BALL_AND_SOCKET),
	  pivot(pivot), pinDir(pinDir),
	  child(child), parent(parent),
	  limited(limited), coneAngle(coneAngle), minTwist(minTwist), maxTwist(maxTwist)
{
	if (limited) {
		CustomLimitBallAndSocket* tmp = new CustomLimitBallAndSocket(pinAndPivot, childBody, parentBody);
		tmp->SetConeAngle(coneAngle);
		tmp->SetTwistAngle(minTwist, maxTwist);
		m_joint = tmp;
	}
	else {
		m_joint = new CustomBallAndSocket(pinAndPivot, childBody, parentBody);
	}

}

__BallAndSocket::~__BallAndSocket()
{
	if (m_joint)
		delete m_joint;
}

void __BallAndSocket::updateMatrix(const Mat4f& inverse, const Mat4f& matrix)
{
	Mat4f tmp = inverse;
	tmp.setW(Vec3f());
	pinDir *= tmp;
	pinDir = pinDir % matrix;

	pivot *= inverse * matrix;
}

void __BallAndSocket::save(const __BallAndSocket& ball, rapidxml::xml_node<>* sibling, rapidxml::xml_document<>* doc)
{
	using namespace rapidxml;

	xml_node<>* node = doc->allocate_node(node_element, "joint");
	sibling->append_node(node);

	// type attribute
	char* pType = doc->allocate_string("ballandsocket");
	xml_attribute<>* attrT = doc->allocate_attribute("type", pType);
	node->append_attribute(attrT);

	// parentID attribute
	int parentID = ball.parent ? ball.parent->getID() : -1;
	char* pParentID = doc->allocate_string(util::toString(parentID));
	xml_attribute<>* attrP = doc->allocate_attribute("parentID", pParentID);
	node->append_attribute(attrP);

	// childID attribute
	int childID = ball.child->getID();
	char* pChildID = doc->allocate_string(util::toString(childID));
	xml_attribute<>* attrC = doc->allocate_attribute("childID", pChildID);
	node->append_attribute(attrC);

	// pivot attribute
	char* pPivot = doc->allocate_string(ball.pivot.str().c_str());
	xml_attribute<>* attrPi = doc->allocate_attribute("pivot", pPivot);
	node->append_attribute(attrPi);

	// pinDir attribute
	char* pPinDir = doc->allocate_string(ball.pinDir.str().c_str());
	xml_attribute<>* attrPD = doc->allocate_attribute("pinDir", pPinDir);
	node->append_attribute(attrPD);

	// limited attribute
	char* pLimited = doc->allocate_string(util::toString(ball.limited));
	xml_attribute<>* attrLi = doc->allocate_attribute("limited", pLimited);
	node->append_attribute(attrLi);

	if (ball.limited) {
		// coneAngle attribute
		char* pConeAngle = doc->allocate_string(util::toString(ball.coneAngle));
		xml_attribute<>* attrCo = doc->allocate_attribute("coneangle", pConeAngle);
		node->append_attribute(attrCo);

		// minTwist attribute
		char* pMinTwist = doc->allocate_string(util::toString(ball.minTwist));
		xml_attribute<>* attrMi = doc->allocate_attribute("mintwist", pMinTwist);
		node->append_attribute(attrMi);

		// maxTwist attribute
		char* pMaxTwist = doc->allocate_string(util::toString(ball.maxTwist));
		xml_attribute<>* attrMa = doc->allocate_attribute("maxtwist", pMaxTwist);
		node->append_attribute(attrMa);
	}
}

BallAndSocket __BallAndSocket::load(const std::list<Object>& list, rapidxml::xml_node<>* node)
{
	using namespace rapidxml;

	Vec3f pivot, pinDir;
	int parentID = -1, childID = -1;

	//type attribute
	xml_attribute<>* attr = node->first_attribute();

	//parentID attribute
	attr = attr->next_attribute();
	parentID = atoi(attr->value());

	//childID attribute
	attr = attr->next_attribute();
	childID = atoi(attr->value());

	//pivot attribute
	attr = attr->next_attribute();
	pivot.assign(attr->value());

	//pinDir attribute
	attr = attr->next_attribute();
	pinDir.assign(attr->value());


	// Get the objects with the required IDs out of the object list
	Object parent, child;
	for (std::list<Object>::const_iterator itr = list.begin(); itr != list.end(); ++itr) {
		if ((*itr)->getID() == parentID)
			parent = *itr;
		if ((*itr)->getID() == childID)
			child = *itr;
	}

	// limited attribute
	attr = attr->next_attribute();
	bool limited = atoi(attr->value());
	float coneAngle = 0.0f, minTwist = 0.0f, maxTwist = 0.0f;

	if (limited) {
		attr = attr->next_attribute();
		coneAngle = atof(attr->value());

		attr = attr->next_attribute();
		minTwist = atof(attr->value());

		attr = attr->next_attribute();
		maxTwist = atof(attr->value());
	}


	BallAndSocket ball = __BallAndSocket::create(pivot, pinDir, child, parent, limited, coneAngle, minTwist, maxTwist);
	return ball;
}

BallAndSocket __BallAndSocket::create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited, float coneAngle, float minTwist, float maxTwist)
{
	__RigidBody* childBody = dynamic_cast<__RigidBody*>(child.get());
	__RigidBody* parentBody = parent ? dynamic_cast<__RigidBody*>(parent.get()) : NULL;

	dMatrix pinAndPivot(GetIdentityMatrix());
	pinAndPivot.m_front = dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f);
	pinAndPivot.m_up = dVector(0.0f, 1.0f, 0.0f, 0.0f);
	pinAndPivot.m_right = pinAndPivot.m_front * pinAndPivot.m_up;
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);

	BallAndSocket result = BallAndSocket(new __BallAndSocket(pivot, pinDir,
			child, parent, pinAndPivot, childBody->m_body, parentBody ? parentBody->m_body : NULL,
			limited, coneAngle, minTwist, maxTwist));
	return result;
}

}
