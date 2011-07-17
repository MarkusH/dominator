/**
 * @author Markus Doellinger, Robert Waury
 * @date Jun 2, 2011
 * @file simulation/joint.cpp
 */

#include <simulation/object.hpp>
#include <simulation/joint.hpp>
#include <iostream>
#include <util/tostring.hpp>

namespace sim {

__Joint::__Joint(Type type, Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent)
	: type(type), pivot(pivot), pinDir(pinDir), child(child), parent(parent)
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
	if( node->first_attribute("type") && std::string(node->first_attribute("type")->value()) == "hinge" ) return __Hinge::load(list, node);
	if( node->first_attribute("type") && std::string(node->first_attribute("type")->value()) == "slider" ) return __Slider::load(list, node);
	if( node->first_attribute("type") && std::string(node->first_attribute("type")->value()) == "ballandsocket" ) return __BallAndSocket::load(list, node);

	Joint result;
	return result;
}

__Hinge::__Hinge(Vec3f pivot, Vec3f pinDir,
		const Object& child, const Object& parent,
		const dMatrix& pinAndPivot,
		const NewtonBody* childBody, const NewtonBody* parentBody,
		bool limited, float minAngle, float maxAngle)
	: __Joint(HINGE, pivot, pinDir, child, parent), CustomHinge(pinAndPivot, childBody, parentBody),
	  limited(limited), minAngle(minAngle), maxAngle(maxAngle)
{
	this->EnableLimits(limited);
	this->SetLimis(minAngle, maxAngle);
}

Hinge __Hinge::create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited, float minAngle, float maxAngle)
{
	__RigidBody* childBody = dynamic_cast<__RigidBody*>(child.get());
	__RigidBody* parentBody = parent ? dynamic_cast<__RigidBody*>(parent.get()) : NULL;
/*
	dMatrix pinAndPivot(GetIdentityMatrix());
	pinAndPivot.m_front = dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f);
	pinAndPivot.m_up = dVector(0.0f, 1.0f, 0.0f, 0.0f);
	pinAndPivot.m_right = pinAndPivot.m_front * pinAndPivot.m_up;
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);
*/
	dMatrix pinAndPivot = dgGrammSchmidt(dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f));
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);

	Hinge result = Hinge(new __Hinge(pivot, pinDir, child, parent, pinAndPivot, childBody->m_body, parentBody ? parentBody->m_body : NULL, limited, minAngle, maxAngle));
	return result;
}

void __Hinge::updateMatrix()
{
	dMatrix matrix0;
	dMatrix matrix1;

	// calculate the position of the pivot point and the Jacobian direction vectors, in global space.
	CalculateGlobalMatrix(m_localMatrix0, m_localMatrix1, matrix0, matrix1);
	pinDir = Vec3f(matrix0.m_front.m_x, matrix0.m_front.m_y, matrix0.m_front.m_z);
	pivot = Vec3f(matrix0.m_posit.m_x, matrix0.m_posit.m_y, matrix0.m_posit.m_z);
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
	bool limited = false;
	float minAngle = 0.0;
	float maxAngle = 0.0;

	//parentID attribute
	xml_attribute<>* attr = node->first_attribute("parentID");
	if(attr) {
	parentID = atoi(attr->value());
	} else throw parse_error("No \"parentID\" attribute in joint tag found", node->name());


	//childID attribute
	attr = node->first_attribute("childID");
	if(attr) {
	childID = atoi(attr->value());
	} else throw parse_error("No \"childID\" attribute in hinge tag found", node->name());


	//pivot attribute
	attr = node->first_attribute("pivot");
	if(attr) {
	pivot.assign(attr->value());
	} else throw parse_error("No \"pivot\" attribute in hinge tag found", node->name());


	//pinDir attribute
	attr = node->first_attribute("pinDir");
	if(attr) {
	pinDir.assign(attr->value());
	} else throw parse_error("No \"pinDir\" attribute in joint tag found", node->name());


	// Get the objects with the required IDs out of the object list
	Object parent, child;
	for (std::list<Object>::const_iterator itr = list.begin(); itr != list.end(); ++itr) {
		if ((*itr)->getID() == parentID)
			parent = *itr;
		if ((*itr)->getID() == childID)
			child = *itr;
	}

	// limited attribute
	attr = node->first_attribute("limited");
	if(attr) {
		limited = atoi(attr->value());
	} else throw parse_error("No \"limited\" attribute in joint tag found", node->name());


	if (limited) {
		attr = node->first_attribute("minangle");
		if(attr) {
		minAngle = atof(attr->value());
		} else throw parse_error("No \"minangle\" attribute in joint tag found", node->name());


		attr = node->first_attribute("maxangle");
		if(attr) {
		maxAngle = atof(attr->value());
		} else throw parse_error("No \"maxangle\" attribute in joint tag found", node->name());

	}

	Hinge hinge = __Hinge::create(pivot, pinDir, child, parent, limited, minAngle, maxAngle);
	return hinge;
}


__Slider::__Slider(Vec3f pivot, Vec3f pinDir,
		const Object& child, const Object& parent,
		const dMatrix& pinAndPivot,
		const NewtonBody* childBody, const NewtonBody* parentBody,
		bool limited, float minDist, float maxDist)
	: __Joint(SLIDER, pivot, pinDir, child, parent), CustomSlider(pinAndPivot, childBody, parentBody),
	  limited(limited), minDist(minDist), maxDist(maxDist)
{
	this->EnableLimits(limited);
	this->SetLimis(minDist, maxDist);
}

Slider __Slider::create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited, float minDist, float maxDist)
{
	__RigidBody* childBody = dynamic_cast<__RigidBody*>(child.get());
	__RigidBody* parentBody = parent ? dynamic_cast<__RigidBody*>(parent.get()) : NULL;
/*
	dMatrix pinAndPivot(GetIdentityMatrix());
	pinAndPivot.m_front = dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f);
	pinAndPivot.m_up = dVector(0.0f, 1.0f, 0.0f, 0.0f);
	pinAndPivot.m_right = pinAndPivot.m_front * pinAndPivot.m_up;
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);
*/
	dMatrix pinAndPivot = dgGrammSchmidt(dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f));
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);

	Slider result = Slider(new __Slider(pivot, pinDir, child, parent,
			pinAndPivot, childBody->m_body, parentBody ? parentBody->m_body : NULL, limited, minDist, maxDist));
	return result;
}

void __Slider::updateMatrix()
{
	dMatrix matrix0;
	dMatrix matrix1;

	// calculate the position of the pivot point and the Jacobian direction vectors, in global space.
	CalculateGlobalMatrix(m_localMatrix0, m_localMatrix1, matrix0, matrix1);
	pinDir = Vec3f(matrix0.m_front.m_x, matrix0.m_front.m_y, matrix0.m_front.m_z);
	pivot = Vec3f(matrix0.m_posit.m_x, matrix0.m_posit.m_y, matrix0.m_posit.m_z);

	// update the distances according to the current position of the pivot
	float dist = (matrix0.m_posit - matrix1.m_posit) % matrix0.m_front;
	minDist -= dist;
	maxDist -= dist;
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
	attr = node->first_attribute("parentID");
	if (attr) {
	parentID = atoi(attr->value());
	} else throw parse_error("No \"parentID\" attribute in joint tag found", node->name());

	//childID attribute
	attr = node->first_attribute("childID");
	if (attr) {
	childID = atoi(attr->value());
	} else throw parse_error("No \"childID\" attribute in joint tag found", node->name());

	//pivot attribute
	attr = node->first_attribute("pivot");
	if (attr) {
	pivot.assign(attr->value());
	} else throw parse_error("No \"pivot\" attribute in joint tag found", node->name());

	//pinDir attribute
	attr = node->first_attribute("pinDir");
	if (attr) {
	pinDir.assign(attr->value());
	} else throw parse_error("No \"pinDir\" attribute in joint tag found", node->name());


	// Get the objects with the required IDs out of the object list
	Object parent, child;
	for (std::list<Object>::const_iterator itr = list.begin(); itr != list.end(); ++itr) {
		if ((*itr)->getID() == parentID)
			parent = *itr;
		if ((*itr)->getID() == childID)
			child = *itr;
	}

	// limited attribute
	bool limited;
	attr = node->first_attribute("limited");
	if (attr) {
	limited = atoi(attr->value());
	} else throw parse_error("No \"limited\" attribute in joint tag found", node->name());

	float minDist = 0.0f, maxDist = 0.0f;

	if (limited) {
		attr = node->first_attribute("mindist");
		if (attr) {
		minDist = atof(attr->value());
		} else throw parse_error("No \"mindist\" attribute in joint tag found", node->name());

		attr = node->first_attribute("maxdist");
		if (attr) {
		maxDist = atof(attr->value());
		} else throw parse_error("No \"maxdist\" attribute in joint tag found", node->name());
	}

	Slider slider = __Slider::create(pivot, pinDir, child, parent, limited, minDist, maxDist);
	return slider;
}


__BallAndSocket::__BallAndSocket(Vec3f pivot, Vec3f pinDir,
		const Object& child, const Object& parent,
		const dMatrix& pinAndPivot,
		const NewtonBody* childBody, const NewtonBody* parentBody,
		bool limited, float coneAngle, float minTwist, float maxTwist)
	: __Joint(BALL_AND_SOCKET, pivot, pinDir, child, parent),
	  limited(limited), coneAngle(coneAngle), minTwist(minTwist), maxTwist(maxTwist)
{
}

__BallAndSocketStd::__BallAndSocketStd(Vec3f pivot, Vec3f pinDir,
		const Object& child, const Object& parent,
		const dMatrix& pinAndPivot,
		const NewtonBody* childBody, const NewtonBody* parentBody,
		bool limited, float coneAngle, float minTwist, float maxTwist)
: __BallAndSocket(pivot, pinDir, child, parent, pinAndPivot, childBody, parentBody,
		limited, coneAngle, minTwist, maxTwist), CustomBallAndSocket(pinAndPivot, childBody, parentBody)
{
}

__BallAndSocketLimited::__BallAndSocketLimited(Vec3f pivot, Vec3f pinDir,
		const Object& child, const Object& parent,
		const dMatrix& pinAndPivot,
		const NewtonBody* childBody, const NewtonBody* parentBody,
		bool limited, float coneAngle, float minTwist, float maxTwist)
	: __BallAndSocket(pivot, pinDir, child, parent, pinAndPivot, childBody, parentBody,
			limited, coneAngle, minTwist, maxTwist), CustomLimitBallAndSocket(pinAndPivot, childBody, parentBody)
{
	SetConeAngle(coneAngle);
	SetTwistAngle(minTwist, maxTwist);
}

__BallAndSocket::~__BallAndSocket()
{
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
	bool limited;

	//parentID attribute
	xml_attribute<>* attr = node->first_attribute("parentID");
	if (attr) {
	parentID = atoi(attr->value());
	} else throw parse_error("No \"parentID\" attribute in joint tag found", node->name());

	//childID attribute
	attr = node->first_attribute("childID");
	if (attr) {
	childID = atoi(attr->value());
	} else throw parse_error("No \"childID\" attribute in joint tag found", node->name());


	//pivot attribute
	attr = node->first_attribute("pivot");
	if (attr) {
	pivot.assign(attr->value());
	} else throw parse_error("No \"pivot\" attribute in joint tag found", node->name());


	//pinDir attribute
	attr = node->first_attribute("pinDir");
	if (attr) {
	pinDir.assign(attr->value());
	} else throw parse_error("No \"pinDir\" attribute in joint tag found", node->name());



	// Get the objects with the required IDs out of the object list
	Object parent, child;
	for (std::list<Object>::const_iterator itr = list.begin(); itr != list.end(); ++itr) {
		if ((*itr)->getID() == parentID)
			parent = *itr;
		if ((*itr)->getID() == childID)
			child = *itr;
	}

	// limited attribute
	attr = node->first_attribute("limited");
	if (attr) {
	limited = atoi(attr->value());
	} else throw parse_error("No \"limited\" attribute in joint tag found", node->name());

	float coneAngle = 0.0f, minTwist = 0.0f, maxTwist = 0.0f;

	if (limited) {
		attr = node->first_attribute("coneangle");
		if (attr) {
		coneAngle = atof(attr->value());
		} else throw parse_error("No \"coneangle\" attribute in joint tag found", node->name());


		attr = node->first_attribute("mintwist");
		if (attr) {
		minTwist = atof(attr->value());
		} else throw parse_error("No \"mintwist\" attribute in joint tag found", node->name());


		attr = node->first_attribute("maxtwist");
		if (attr) {
		maxTwist = atof(attr->value());
		} else throw parse_error("No \"maxtwist\" attribute in joint tag found", node->name());

	}


	BallAndSocket ball = __BallAndSocket::create(pivot, pinDir, child, parent, limited, coneAngle, minTwist, maxTwist);
	return ball;
}

BallAndSocket __BallAndSocket::create(Vec3f pivot, Vec3f pinDir, const Object& child, const Object& parent, bool limited, float coneAngle, float minTwist, float maxTwist)
{
	__RigidBody* childBody = dynamic_cast<__RigidBody*>(child.get());
	__RigidBody* parentBody = parent ? dynamic_cast<__RigidBody*>(parent.get()) : NULL;
/*
	dMatrix pinAndPivot(GetIdentityMatrix());
	pinAndPivot.m_front = dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f);
	pinAndPivot.m_up = dVector(0.0f, 1.0f, 0.0f, 0.0f);
	pinAndPivot.m_right = pinAndPivot.m_front * pinAndPivot.m_up;
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);
*/

	dMatrix pinAndPivot = dgGrammSchmidt(dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f));
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);

	BallAndSocket result;
	if (limited) {
		result = BallAndSocket(new __BallAndSocketLimited(pivot, pinDir, child, parent, pinAndPivot, childBody->m_body,
				parentBody ? parentBody->m_body : NULL, limited, coneAngle, minTwist, maxTwist));
	} else {
		result = BallAndSocket(new __BallAndSocketStd(pivot, pinDir, child, parent, pinAndPivot, childBody->m_body,
				parentBody ? parentBody->m_body : NULL, limited, coneAngle, minTwist, maxTwist));
	}

	return result;
}

void __BallAndSocketStd::updateMatrix()
{
	dMatrix matrix0;
	dMatrix matrix1;

	// calculate the position of the pivot point and the Jacobian direction vectors, in global space.
	CalculateGlobalMatrix(m_localMatrix0, m_localMatrix1, matrix0, matrix1);

	pinDir = Vec3f(matrix0.m_front.m_x, matrix0.m_front.m_y, matrix0.m_front.m_z);
	pivot = Vec3f(matrix0.m_posit.m_x, matrix0.m_posit.m_y, matrix0.m_posit.m_z);
}

void __BallAndSocketLimited::updateMatrix()
{
	dMatrix matrix0;
	dMatrix matrix1;

	// calculate the position of the pivot point and the Jacobian direction vectors, in global space.
	CalculateGlobalMatrix(m_localMatrix0, m_localMatrix1, matrix0, matrix1);
	pinDir = Vec3f(matrix0.m_front.m_x, matrix0.m_front.m_y, matrix0.m_front.m_z);
	pivot = Vec3f(matrix0.m_posit.m_x, matrix0.m_posit.m_y, matrix0.m_posit.m_z);
}


}
