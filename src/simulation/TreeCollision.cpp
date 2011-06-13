/*
 * TreeCollision.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/TreeCollision.hpp>
#include <simulation/Object.hpp>
#include <simulation/Compound.hpp>
#include <simulation/Material.hpp>
#include <simulation/Simulation.hpp>
#include <newton/util.hpp>
#include <iostream>
#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>
#include <lib3ds/types.h>

#define OCTREE_NODE_SIZE 50

namespace sim {

__TreeCollision::Node::Node(__TreeCollision* tree, Vec3f pos, float size, std::vector<uint32_t>& parentIndices)
	: tree(tree), pos(pos), size(size)
{
	for (unsigned i = 0; i < parentIndices.size(); i += 3) {
		// if triangle in node then add to indices and remove from parent
		if (inside(&parentIndices[i])) {
			indices.push_back(parentIndices[i]);
			indices.push_back(parentIndices[i+1]);
			indices.push_back(parentIndices[i+2]);
			parentIndices.erase(parentIndices.begin() + i, parentIndices.begin() + i + 3);
			i -= 3;
		}
	}

	if (indices.size() > OCTREE_NODE_SIZE) {
		Node* node;
		node = new Node(tree, pos + Vec3f(-size, -size,  size) * 0.5f, size * 0.5f, indices); childs.push_back(node);
		node = new Node(tree, pos + Vec3f(-size, -size, -size) * 0.5f, size * 0.5f, indices); childs.push_back(node);
		node = new Node(tree, pos + Vec3f( size, -size,  size) * 0.5f, size * 0.5f, indices); childs.push_back(node);
		node = new Node(tree, pos + Vec3f( size, -size, -size) * 0.5f, size * 0.5f, indices); childs.push_back(node);

		node = new Node(tree, pos + Vec3f(-size,  size,  size) * 0.5f, size * 0.5f, indices); childs.push_back(node);
		node = new Node(tree, pos + Vec3f(-size,  size, -size) * 0.5f, size * 0.5f, indices); childs.push_back(node);
		node = new Node(tree, pos + Vec3f( size,  size,  size) * 0.5f, size * 0.5f, indices); childs.push_back(node);
		node = new Node(tree, pos + Vec3f( size,  size, -size) * 0.5f, size * 0.5f, indices); childs.push_back(node);

		for (unsigned i = 0; i < childs.size(); ++i) {
			if (childs[i]->isEmpty()) {
				delete childs[i];
				childs.erase(childs.begin() + i--);
			}
		}
	}
}

bool __TreeCollision::Node::isEmpty() {
	for (unsigned i = 0; i < childs.size(); ++i)
		if (!childs[i]->isEmpty())
			return false;
	return indices.size() == 0;
}

bool __TreeCollision::Node::inside(uint32_t* i0) {
	const Vec3f sz(size, size, size);
	for (unsigned i = 0; i < 3; ++i) {
		Vec3f v(&tree->m_data[i0[i] * 3]);
		if (v >= (pos - sz) &&
			v <= (pos + sz))
			return true;
	}
	return false;
}

void __TreeCollision::Node::drawWireFrame() {
	Vec3f min = pos, max = pos;
	min -= Vec3f(size, size, size);
	max += Vec3f(size, size, size);
	if (Simulation::instance().getCamera().testAABB(min, max) == 1)
		glColor3f(1.0f, 1.0f, 0.0f);
	else
		glColor3f(1.0f, 0.0f, 1.0f);
      glVertex3f(pos.x-size,pos.y-size,pos.z-size);
      glVertex3f(pos.x+size,pos.y-size,pos.z-size);
      glVertex3f(pos.x-size,pos.y+size,pos.z-size);
      glVertex3f(pos.x+size,pos.y+size,pos.z-size);
      glVertex3f(pos.x-size,pos.y-size,pos.z+size);
      glVertex3f(pos.x+size,pos.y-size,pos.z+size);
      glVertex3f(pos.x-size,pos.y+size,pos.z+size);
      glVertex3f(pos.x+size,pos.y+size,pos.z+size);

      glVertex3f(pos.x+size,pos.y+size,pos.z+size);
      glVertex3f(pos.x+size,pos.y-size,pos.z+size);
      glVertex3f(pos.x+size,pos.y+size,pos.z-size);
      glVertex3f(pos.x+size,pos.y-size,pos.z-size);
      glVertex3f(pos.x-size,pos.y+size,pos.z+size);
      glVertex3f(pos.x-size,pos.y-size,pos.z+size);
      glVertex3f(pos.x-size,pos.y+size,pos.z-size);
      glVertex3f(pos.x-size,pos.y-size,pos.z-size);

      glVertex3f(pos.x+size,pos.y+size,pos.z+size);
      glVertex3f(pos.x+size,pos.y+size,pos.z-size);
      glVertex3f(pos.x+size,pos.y-size,pos.z+size);
      glVertex3f(pos.x+size,pos.y-size,pos.z-size);
      glVertex3f(pos.x-size,pos.y-size,pos.z+size);
      glVertex3f(pos.x-size,pos.y-size,pos.z-size);
      glVertex3f(pos.x-size,pos.y+size,pos.z+size);
      glVertex3f(pos.x-size,pos.y+size,pos.z-size);
		for (unsigned i = 0; i < childs.size(); ++i) {
			childs[i]->drawWireFrame();
		}
}


__TreeCollision::Node::~Node()
{
	for (std::vector<Node*>::iterator itr = childs.begin(); itr != childs.end(); ++itr) {
		delete *itr;
	}
}


__TreeCollision::__TreeCollision(const Mat4f& matrix, const std::string& fileName)
	: __Object(TREE_COLLISION), Body(matrix), m_node(NULL)
{

	/*
	 * There are n meshes, each mesh has a global normal, uv, and vertex array
	 * Each face has 3 indices and a material
	 *
	 * make a T2F_N3F_V3F interleaved array of all meshes and a global index array
	 *
	 * Foreach face store indexOffset and indexCount (is it always 3?) and material
	 * Sort
	 *
	 * Or immediately in ocree? with displaylists, should be better to implement
	 */

	/// @todo implement index array for 3ds data
	Lib3dsFile* file = lib3ds_file_load(fileName.c_str());
	if (!file)
		return;

	const NewtonWorld* world = Simulation::instance().getWorld();
	int defaultMaterial = MaterialMgr::instance().getID("yellow");
	int numFaces = 0;

	// count the faces
	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next)
		numFaces += mesh->faces;

	m_vertexCount = numFaces*3;
	m_vertices = new Lib3dsVector[numFaces * 3];
	m_normals = new Lib3dsVector[numFaces * 3];
	m_uvs = new Lib3dsTexel[numFaces * 3];

	m_data.reserve(3 * numFaces * 3);
	m_indices.reserve(3 * numFaces);

	unsigned finishedFaces = 0;

	NewtonCollision* collision = NewtonCreateTreeCollision(world, defaultMaterial);
	NewtonTreeCollisionBeginBuild(collision);

	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
		//data.reserve(data.size() + (mesh->points * (3 + 3 + 2)));
		//data.resize(data.size() + (mesh->points * (3 + 3 + 2)));
		int faceMaterial = defaultMaterial;
		lib3ds_mesh_calculate_normals(mesh, &m_normals[finishedFaces*3]);
		for(unsigned cur_face = 0; cur_face < mesh->faces; cur_face++) {
			Lib3dsFace* face = &mesh->faceL[cur_face];
			for(unsigned int i = 0;i < 3; i++) {
				memcpy(&m_vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
				//memcpy(&m_uvs[finishedFaces*3 + i], mesh->texelL[face->points[i]], sizeof(Lib3dsTexel));

				m_data.push_back(mesh->pointL[face->points[i]].pos[0]);
				m_data.push_back(mesh->pointL[face->points[i]].pos[1]);
				m_data.push_back(mesh->pointL[face->points[i]].pos[2]);
				m_indices.push_back(m_indices.size());
			}
			faceMaterial = face->material && face->material[0] ? MaterialMgr::instance().getID(face->material) : defaultMaterial;
			NewtonTreeCollisionAddFace(collision, 3, m_vertices[finishedFaces*3], sizeof(Lib3dsVector), faceMaterial);
			finishedFaces++;
		}
	}
	lib3ds_file_free(file);
	NewtonTreeCollisionEndBuild(collision, 1);

	this->create(collision, 0.0f);
	//NewtonBodySetContinuousCollisionMode(m_body, 1);
	NewtonReleaseCollision(world, collision);
}

__TreeCollision::~__TreeCollision()
{
	if (m_node) delete m_node;
	if (m_vertices) delete m_vertices;
	if (m_normals) delete m_normals;
	if (m_uvs) delete m_uvs;
}

void __TreeCollision::createOctree()
{
	if (m_data.size() == 0)
		return;

	float min = m_data[0], max = m_data[0];

	for (unsigned i = 0; i < m_data.size() / 3; i += 3) {
		min = min(min, m_data[i * 3 + 0]);
		max = max(max, m_data[i * 3 + 0]);
		min = min(min, m_data[i * 3 + 1]);
		max = max(max, m_data[i * 3 + 1]);
		min = min(min, m_data[i * 3 + 2]);
		max = max(max, m_data[i * 3 + 2]);
	}

	float p = (min + max) * 0.5f;
	Vec3f pos(p, p, p);

	float size = (max - min) * 0.5f;

	std::vector<uint32_t> indices(m_indices);
	m_node = new Node(this, pos, size, indices);
}

void __TreeCollision::genBuffers(ogl::VertexBuffer& vbo)
{
	// get the offset in floats and vertices
	const unsigned vertexSize = vbo.floatSize();
	const unsigned byteSize = vbo.byteSize();
	const unsigned floatOffset = vbo.m_data.size();
	const unsigned vertexOffset = floatOffset / vertexSize;

	NewtonCollision* collision = NewtonBodyGetCollision(m_body);

	// create a mesh from the collision
	NewtonMesh* collisionMesh = NewtonMeshCreateFromCollision(collision);

	switch (m_type) {
	case SPHERE:
		NewtonMeshApplySphericalMapping(collisionMesh, 0);
		break;
	case CYLINDER:
	case CHAMFER_CYLINER:
		//NewtonMeshApplyCylindricalMapping(collisionMesh, 0, 0);
		//break;
	case BOX:
	default:
		NewtonMeshApplyBoxMapping(collisionMesh, 0, 0, 0);
		break;
	}

	//NewtonMeshCalculateVertexNormals(collisionMesh, 60.0f * 3.1416f/180.0f);


	// allocate the vertex data
	int vertexCount = NewtonMeshGetPointCount(collisionMesh);

	vbo.m_data.reserve(floatOffset + vertexCount * vertexSize);
	vbo.m_data.resize(floatOffset + vertexCount * vertexSize);

	NewtonMeshGetVertexStreams(collisionMesh,
			byteSize, &vbo.m_data[floatOffset + 2 + 3],
			byteSize, &vbo.m_data[floatOffset + 2],
			byteSize, &vbo.m_data[floatOffset],
			byteSize, &vbo.m_data[floatOffset]);

	//std::cout << "____" << std::endl;
	//std::cout << vertexCount << std::endl;


	// iterate over the submeshes and store the indices
	void* const meshCookie = NewtonMeshBeginHandle(collisionMesh);
	for (int handle = NewtonMeshFirstMaterial(collisionMesh, meshCookie);
			handle != -1; handle = NewtonMeshNextMaterial(collisionMesh, meshCookie, handle)) {

		//int material = NewtonMeshMaterialGetMaterial(collisionMesh, meshCookie, handle);
		//std::cout << "material " << material << std::endl;
		//std::cout << "mesh " << material << std::endl;

		// create a new submesh
		ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
		subBuffer->material = "yellow";
		subBuffer->object = this;

		subBuffer->dataCount = vertexCount;
		subBuffer->dataOffset = vertexOffset;

		// get the indices
		subBuffer->indexCount = NewtonMeshMaterialGetIndexCount(collisionMesh, meshCookie, handle);
		//std::cout << subBuffer->indexCount << std::endl;
		uint32_t* indices = new uint32_t[subBuffer->indexCount];
		NewtonMeshMaterialGetIndexStream(collisionMesh, meshCookie, handle, (int*)indices);
//std::cout << "indices " << subBuffer->indexCount << std::endl;
		subBuffer->indexOffset = vbo.m_indices.size();

		// copy the indices to the global list and add the offset
		vbo.m_indices.reserve(vbo.m_indices.size() + subBuffer->indexCount);
		for (unsigned i = 0; i < subBuffer->indexCount; ++i)
			vbo.m_indices.push_back(vertexOffset + indices[i]);

		delete indices;
		vbo.m_buffers.push_back(subBuffer);
	}
	NewtonMeshEndHandle(collisionMesh, meshCookie);

	NewtonMeshDestroy(collisionMesh);
}

bool __TreeCollision::contains(const NewtonBody* const body)
{
	return m_body == body;
}

bool __TreeCollision::contains(const __Object* object)
{
	if (object == this)
		return true;
	return false;
}


void __TreeCollision::render()
{
	if (NewtonBodyGetSleepState(m_body))
		glColor3f(1.0f, 1.0f, 0.0f);
	else
		glColor3f(1.0f, 0.0f, 0.0f);
	newton::showCollisionShape(getCollision(), m_matrix);
	glBegin(GL_LINES);
	if (m_node)
		m_node->drawWireFrame();
	glEnd();
}

}
