/*
 * TreeCollision.hpp
 *
 *  Created on: Jun 5, 2011
 *      Author: Markus Doellinger
 */

#ifndef TREECOLLISION_HPP_
#define TREECOLLISION_HPP_

#include <simulation/Object.hpp>

namespace sim {

class __TreeCollision : public __Object, public Body {
protected:
	int m_vertexCount;
	Lib3dsVector* m_vertices;
	Lib3dsVector* m_normals;
	Lib3dsTexel* m_uvs;
public:
	__TreeCollision(const Mat4f& matrix, const std::string& fileName);
	~__TreeCollision();

	virtual const Mat4f& getMatrix() { return Body::getMatrix(); }
	virtual void setMatrix(const Mat4f& matrix) { Body::setMatrix(matrix); }

	virtual float convexCastPlacement(bool apply = true) { return 0.0f; };

	virtual bool contains(const NewtonBody* const body);
	virtual bool contains(const __Object* object);

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	virtual void render();
};
}

#endif /* TREECOLLISION_HPP_ */
