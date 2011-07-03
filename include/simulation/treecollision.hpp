/*
 * TreeCollision.hpp
 *
 *  Created on: Jun 5, 2011
 *      Author: Markus Doellinger
 */

#ifndef TREECOLLISION_HPP_
#define TREECOLLISION_HPP_

#include <simulation/object.hpp>

namespace sim {

class __TreeCollision;
typedef std::tr1::shared_ptr<__TreeCollision> TreeCollision;

class __TreeCollision : public __Object, public Body {
protected:

	struct Node {
		__TreeCollision* tree;
		std::vector<Node*> childs;
		std::vector<uint32_t> indices;
		Vec3f pos;
		float size;
		GLuint list;

		Node(__TreeCollision* tree, Vec3f pos, float size, std::vector<uint32_t>& parentIndices);
		~Node();

		bool isEmpty();
		bool inside(uint32_t* i0);
		int drawWireFrame(bool test = true);
		int draw(bool test = true);
	};

	std::string m_fileName;
	int m_nodeCount;
	Node* m_node;
	// T2F_N3F_V3F
	std::vector<float> m_data;
	std::vector<uint32_t> m_indices;
NewtonMesh* m_mesh;
	int m_vertexCount;
	Lib3dsVector* m_vertices;
	Lib3dsVector* m_normals;
	Lib3dsTexel* m_uvs;
	GLuint m_list;
public:
	__TreeCollision(const Mat4f& matrix, const std::string& fileName);
	~__TreeCollision();

	virtual const Mat4f& getMatrix() const { return Body::getMatrix(); }
	virtual void setMatrix(const Mat4f& matrix) { Body::setMatrix(matrix); }

	virtual void getAABB(Vec3f& min, Vec3f& max) { NewtonBodyGetAABB(m_body, &min[0], &max[0]); }

	virtual float convexCastPlacement(bool apply = true, std::list<NewtonBody*>* noCollision = NULL) { return 0.0f; };

	virtual bool contains(const NewtonBody* const body);
	virtual bool contains(const __Object* object);

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	virtual void createOctree();
	virtual void render();

	/**
	 * Saves TreeCollision object to XML
	 *
	 * @param object	Reference to object to save
	 * @param node		Pointer to root node
	 * @param doc		Pointer to XML document
	 */
	static void save(__TreeCollision& object, rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc);

	/**
	 * Loads Compound from XML node
	 *
	 * @param	node	Pointer to XML node
	 * @throws	rapidxml::parse_error	Attribute not found
	 * @return	TreeCollision object
	 */
	static TreeCollision load(rapidxml::xml_node<>* node);
};
}

#endif /* TREECOLLISION_HPP_ */
