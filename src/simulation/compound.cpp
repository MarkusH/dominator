/**
 * @author Markus Doellinger, Robert Waury
 * @date Jun 1, 2011
 * @file simulation/compound.cpp
 */

#include <simulation/compound.hpp>

namespace sim {

__Compound::__Compound()
	: __Object(COMPOUND), m_nextID(0)
{
	m_matrix = Mat4f::identity();
}

__Compound::__Compound(const Mat4f& matrix)
	: __Object(COMPOUND), m_nextID(0)
{
	m_matrix = matrix;
}

__Compound::__Compound(const Vec3f& position)
	: __Object(COMPOUND), m_nextID(0)
{
	m_matrix = Mat4f::translate(position);
}

__Compound::~__Compound()
{
#ifdef _DEBUG
	std::cout << "delete compound" << std::endl;
#endif
	//m_nodes.clear();
	//m_joints.clear();
}

Compound __Compound::createCompound(const Mat4f& matrix)
{
	Compound result(new __Compound());
	return result;
}

void __Compound::getAABB(Vec3f& min, Vec3f& max)
{
	if (m_nodes.size() > 0) {
		m_nodes.front()->getAABB(min, max);
		std::list<Object>::iterator itr = m_nodes.begin();
		Vec3f _min, _max;
		for (++itr ; itr != m_nodes.end(); ++itr) {
			(*itr)->getAABB(_min, _max);
			min.x = min(min.x, _min.x);
			min.y = min(min.y, _min.y);
			min.z = min(min.z, _min.z);
			max.x = max(max.x, _max.x);
			max.y = max(max.y, _max.y);
			max.z = max(max.z, _max.z);
		}
	} else {
		min = max = Vec3f();
	}
}

float __Compound::convexCastPlacement(bool apply, std::list<NewtonBody*>* noCollision)
{
	std::list<NewtonBody*> temp;
	for (std::list<Object>::iterator itr = m_nodes.begin(); itr != m_nodes.end(); ++itr) {
		temp.push_back(((__RigidBody*)itr->get())->m_body);
	}
	float maximum = -1000.0f;
	for (std::list<Object>::iterator itr = m_nodes.begin(); itr != m_nodes.end(); ++itr) {
		float current = (*itr)->convexCastPlacement(false, &temp);
		current += (m_matrix._42 - (*itr)->getMatrix()._42);
		if (current > maximum) maximum = current;
	}
	Mat4f matrix = m_matrix;
	matrix._42 = maximum + 0.0001f;
	if (apply)
		setMatrix(matrix);
	return matrix._42;
}

void __Compound::save(__Compound& compound, rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc)
{
	// foreach child_node
	for (std::list<Object>::iterator itr = compound.m_nodes.begin();
				itr != compound.m_nodes.end(); ++itr) {
					__Object::save(*itr->get(), node, doc);
	}

	// foreach joint
	for (std::list<Joint>::iterator itr = compound.m_joints.begin(); itr != compound.m_joints.end(); ++itr) {
		(*itr)->updateMatrix();
		__Joint::save(*itr->get(), node, doc);
	}
}

Compound __Compound::load(rapidxml::xml_node<>* nodes)
{
	using namespace rapidxml;

	Compound result = Compound(new __Compound());

	// id attribute
	//xml_attribute<>* attr = nodes->first_attribute();

	// matrix attribute
	xml_attribute<>* attr = nodes->first_attribute("matrix");

	// Cache the matrix because the subsequent add() method would
	// multiply the matrix of the nodes with the matrix of the
	// compound. This would not be correct because the matrix of
	// the nodes is already in global space
	Mat4f matrix;
	if(attr) {
	matrix.assign(attr->value());
	result->m_matrix = Mat4f::identity();
	} else throw parse_error("No \"matrix\" attribute in compound tag found", nodes->value());

	// optional compound freeze state
	attr = nodes->first_attribute("freezeState");
	int freezeState = attr ? atoi(attr->value()) : 0;
	
	for (xml_node<>* node = nodes->first_node(); node; node = node->next_sibling()) {
		std::string type = node->name();
		if(type == "object") {
			Object obj = __Object::load(node);
	 		result->add(obj);
		}
		if(type == "joint") {
			//std::list<Object>& list = std::list<Object>(result->m_nodes);
			Joint joint = __Joint::load(result->m_nodes, node);
	 		result->m_joints.push_back(joint);
		}
	}

	// we have to set that after loading all nodes because when attaching
	// joints, all states previously set get lost
	if (freezeState) {
		for (std::list<Object>::iterator itr = result->m_nodes.begin();
					itr != result->m_nodes.end(); ++itr) {
			(*itr)->setFreezeState(freezeState);
		}
	}


	result->m_matrix = matrix;

	// foreach element "object" in node
	// 		Object obj = __Object::load(element)
	// 		result->add(obj)

	// foreach element "joint" in node
	// 		Joint joint = __Joint::load(element)
	// 		result->m_joints.push_back(joint)

	return result;
}

Hinge __Compound::createHinge(const Vec3f& pivot, const Vec3f& pinDir, const Object& child, const Object& parent, bool limited, float minAngle, float maxAngle)
{
	if (child && child != parent) {
		Vec3f pin = pinDir % m_matrix;
		Hinge hinge = __Hinge::create(pivot, pin, child, parent, limited, minAngle, maxAngle);
		m_joints.push_back(hinge);
		return hinge;
	}
	Hinge result;
	return result;
}


Slider __Compound::createSlider(const Vec3f& pivot, const Vec3f& pinDir, const Object& child, const Object& parent,
		bool limited, float minDist, float maxDist)
{
	if (child && child != parent) {
		Vec3f pin = pinDir % m_matrix;
		Slider slider = __Slider::create(pivot, pin, child, parent, limited, minDist, maxDist);
		m_joints.push_back(slider);
		return slider;
	}
	Slider result;
	return result;
}

BallAndSocket __Compound::createBallAndSocket(const Vec3f& pivot, const Vec3f& pinDir, const Object& child, const Object& parent, bool limited, float coneAngle, float minTwist, float maxTwist)
{
	if (child && child != parent) {
		Vec3f pin = pinDir % m_matrix;
		BallAndSocket ball = __BallAndSocket::create(pivot, pin, child, parent, limited, coneAngle, minTwist, maxTwist);
		m_joints.push_back(ball);
		return ball;
	}
	BallAndSocket result;
	return result;
}

void __Compound::add(Object object)
{
	object->setID(m_nextID++);
	Mat4f matrix = object->getMatrix() * m_matrix;
	object->setMatrix(matrix);
	m_nodes.push_back(object);
}

void __Compound::setMatrix(const Mat4f& matrix)
{
	// We have to renew the matrix of all nodes by
	// untransforming them first with the old matrix
	// and then transforming them with the new one
	Mat4f inverse = m_matrix.inverse();
	for (std::list<Object>::iterator itr = m_nodes.begin();
				itr != m_nodes.end(); ++itr) {
		Mat4f newMatrix = (*itr)->getMatrix();
		newMatrix *= inverse * matrix;
		(*itr)->setMatrix(newMatrix);
	}
	m_matrix = matrix;
}

void __Compound::setFreezeState(int state)
{
	for (std::list<Object>::iterator itr = m_nodes.begin();
				itr != m_nodes.end(); ++itr) {
		(*itr)->setFreezeState(state);
	}
}


int __Compound::getFreezeState()
{
	int state = (*m_nodes.begin())->getFreezeState();
	for (std::list<Object>::iterator itr = m_nodes.begin();
				itr != m_nodes.end(); ++itr) {
		if ((*itr)->getFreezeState() != state)
			return 0;
	}
	return state;
}

bool __Compound::contains(const NewtonBody* const body)
{
	for (std::list<Object>::iterator itr = m_nodes.begin();
			itr != m_nodes.end(); ++itr) {
		if ((*itr)->contains(body))
			return true;
	}
	return false;
}

bool __Compound::contains(const __Object* object)
{
	if (object == this)
		return true;
	for (std::list<Object>::iterator itr = m_nodes.begin();
			itr != m_nodes.end(); ++itr) {
		if ((*itr)->contains(object))
			return true;
	}
	return false;
}

void __Compound::genBuffers(ogl::VertexBuffer& vbo)
{
	for (std::list<Object>::iterator itr = m_nodes.begin();
			itr != m_nodes.end(); ++itr) {
		(*itr)->genBuffers(vbo);
	}
}

void __Compound::render()
{
	for (std::list<Object>::iterator itr = m_nodes.begin();
			itr != m_nodes.end(); ++itr) {
		(*itr)->render();
	}
}

}
