/*
 * Compound.cpp
 *
 *  Created on: Jun 1, 2011
 *      Author: markus
 */

#include <simulation/Compound.hpp>

namespace sim {

__Compound::__Compound()
	: __Object(COMPOUND)
{
	m_matrix = Mat4f::identity();
}

__Compound::__Compound(const Mat4f& matrix)
	: __Object(COMPOUND)
{
	m_matrix = matrix;
}

__Compound::__Compound(const Vec3f& position)
	: __Object(COMPOUND)
{
	m_matrix = Mat4f::translate(position);
}

__Compound::~__Compound()
{
	m_nodes.clear();
}

void __Compound::save(const __Compound& compound /* node */)
{
	// set attribute matrix to matrix.str()

	// id = 0
	// foreach child_node
	// 		__Object::save(id++, child_node, node)

	// foreach joint
	// 		__Joint::save(joint, node)
}

Compound __Compound::load(/*node */)
{
	Compound result = Compound(new __Compound());
	// load matrix

	// int id
	// foreach element "object" in node
	// 		Object obj = __Object::load(id, element)
	// 		result->add(obj)

	// foreach element "joint" in node
	// 		Joint joint = __Joint::load(id, element)
	// 		joint->create()
	// 		result->add(obj)

	return result;
}

void __Compound::add(Object object)
{
	Mat4f matrix = object->getMatrix() * m_matrix;
	object->setMatrix(matrix);
	m_nodes.push_back(object);
}

void __Compound::setMatrix(const Mat4f& matrix)
{
	Mat4f inverse = m_matrix.inverse();
	for (std::list<Object>::iterator itr = m_nodes.begin();
				itr != m_nodes.end(); ++itr) {
		Mat4f newMatrix = (*itr)->getMatrix();
		newMatrix *= inverse * matrix;
		(*itr)->setMatrix(newMatrix);
	}
	m_matrix = matrix;
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

bool __Compound::contains(const Object& object)
{
	if (object.get() == this)
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
