/*
 * Joint.cpp
 *
 *  Created on: Jun 2, 2011
 *      Author: markus
 */

#include <simulation/Object.hpp>
#include <simulation/Joint.hpp>
#include <iostream>


namespace sim {

__Joint::__Joint(Type type)
	: type(type)
{

}

void __Joint::save(const __Joint& joint, std::list<Object> list /*node*/)
{
	// generate child node "joint" and append it to node

	switch (joint.type) {
	case HINGE:
		// set attribute to "hinge"
		__Hinge::save((const __Hinge&)joint, list/*, generatedNode*/);
		return;
	}
}

Joint __Joint::load(std::list<Object> list /*node*/ )
{
	// get type of node

	// if type == "hinge"
	// 		return Hinge::load(list, node);

	Joint result;
	return result;
}

__Hinge::__Hinge(Vec3f pivot, Vec3f pinDir, const dMatrix& pinAndPivot, const NewtonBody* child, const NewtonBody* parent)
	: __Joint(HINGE), CustomHinge(pinAndPivot, child, parent), pivot(pivot), pinDir(pinDir)
{

}

Hinge __Hinge::create(Vec3f pivot, Vec3f pinDir, const NewtonBody* child, const NewtonBody* parent)
{
	dMatrix pinAndPivot(GetIdentityMatrix());
	pinAndPivot.m_front = dVector(pinDir.x, pinDir.y, pinDir.z, 0.0f);
	pinAndPivot.m_up = dVector(0.0f, 1.0f, 0.0f, 0.0f);
	pinAndPivot.m_right = pinAndPivot.m_front * pinAndPivot.m_up;
	pinAndPivot.m_posit = dVector(pivot.x, pivot.y, pivot.z, 1.0f);

	Hinge result = Hinge(new __Hinge(pivot, pinDir, pinAndPivot, child, parent));
	return result;
}

void __Hinge::save(const __Hinge& hinge, std::list<Object> list /* node */)
{
	// We need to get the NewtonBody* pointers of the parent compound
	// To do so, we iterate over the given node list of the compound
	// and check whether the child/parent body of the hinge is contained
	// in one of the compound nodes.
	int count = 0;
	int parentID = -1;
	int childID = -1;
	for (std::list<Object>::iterator itr = list.begin(); itr != list.end(); ++itr) {
		if ((*itr)->contains(hinge.GetBody0()))
			childID = count;
		if ((*itr)->contains(hinge.GetBody1()))
			parentID = count;
		count++;
	}

	// set attribute "parentID" to parentID
	// set attribute "childID" to  childID
	// set attribute "pivot" to  hinge.pivot.str()
	// set attribute "pinDir" to  hinge.pinDir.str()
}

Hinge __Hinge::load(/* node */ std::list<Object> list)
{
	Vec3f pivot, pinDir;
	int parentID = -1, childID = -1;

	// parentID = attribute "parentID"
	// childID = attribute "childID"
	// pivot.assign(attribute "pivot")
	// pinDir.assign(attribute "pinDir")

	// We need to get the NewtonBody* pointers of the nodes
	// described by the parent and child IDs. To do so, we
	// iterate over the compound nodes and get the objects
	Object parent, child;
	int count = 0;
	for (std::list<Object>::iterator itr = list.begin(); itr != list.end(); ++itr) {
		if (count == parentID)
			parent = *itr;
		if (count == childID)
			child = *itr;
		count++;
	}

	// We need to get the NewtonBody* pointers of the objects we found, so
	// we cast them to RigidBodies and get the pointer
	__RigidBody* childBody = dynamic_cast<__RigidBody*>(child.get());
	__RigidBody* parentBody = parent ? dynamic_cast<__RigidBody*>(parent.get()) : NULL;

	Hinge hinge = __Hinge::create(pivot, pinDir, childBody->m_body, parentBody ? parentBody->m_body : NULL);
	return hinge;
}

}
