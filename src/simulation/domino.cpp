/*
 * Domino.cpp
 *
 *  Created on: Jun 7, 2011
 *      Author: markus
 */

#include <simulation/domino.hpp>
#include <newton/util.hpp>
#include <simulation/simulation.hpp>
#include <simulation/material.hpp>

namespace sim {

NewtonCollision* __Domino::s_domino_collision[3] = { NULL, NULL, NULL };
Vec3f __Domino::s_domino_size[3] = { Vec3f(3.0f, 8.0f, 0.5f) * 0.75f, Vec3f(3.0f, 8.0f, 0.5f), Vec3f(3.0f, 8.0f, 0.5f) * 1.25f };


__Domino::__Domino(Type type, const Mat4f& matrix, const std::string& material, int freezeState)
	: __RigidBody(type, matrix, material, freezeState, Vec4f(0.1f, 0.1f, 0.1f, 0.1f))
{

}

__Domino::~__Domino()
{
}

#ifndef CONVEX_DOMINO
NewtonCollision*  __Domino::getCollision(Type type, int materialID)
{
	const NewtonWorld* world = Simulation::instance().getWorld();
	Mat4f identity = Mat4f::identity();
	Vec3f size = s_domino_size[type];
	if (!s_domino_collision[type]) {
		s_domino_collision[type] = NewtonCreateBox(world, size.x, size.y, size.z, materialID, identity[0]);
	}
	return s_domino_collision[type];
}

void __Domino::freeCollisions()
{
	const NewtonWorld* world = Simulation::instance().getWorld();
	for (int i = 0; i <= __Domino::DOMINO_LARGE; ++i) {
		if (s_domino_collision[i]) {
			NewtonReleaseCollision(world, s_domino_collision[i]);
			s_domino_collision[i] = NULL;
		}
	}
}
#endif

#ifndef CONVEX_DOMINO
void __Domino::genBuffers(ogl::VertexBuffer& vbo)
{
	//__RigidBody::genBuffers(vbo);

//return;

	// the first three buffers of the vbo are the small, middle and large dominos
	ogl::SubBuffers::iterator itr = vbo.m_buffers.begin();
	for (int i = 0; i < m_type; ++i)
		++itr;

	ogl::SubBuffer* buffer = new ogl::SubBuffer();
	buffer->dataCount = (*itr)->dataCount;
	buffer->dataOffset = (*itr)->dataOffset;
	buffer->indexCount = (*itr)->indexCount;
	buffer->indexOffset = (*itr)->indexOffset;
	buffer->userData = this;
	buffer->material = m_material;
	vbo.m_buffers.push_back(buffer);

}

void __Domino::genDominoBuffers(ogl::VertexBuffer& vbo)
{
	for (int i = DOMINO_SMALL; i <= DOMINO_LARGE; ++i) {
		// get the offset in floats and vertices
		const unsigned vertexSize = vbo.floatSize();
		const unsigned byteSize = vbo.byteSize();
		const unsigned floatOffset = vbo.m_data.size();
		const unsigned vertexOffset = floatOffset / vertexSize;

		NewtonCollision* collision = getCollision((Type)i, 0);

		// create a mesh from the collision
		NewtonMesh* collisionMesh = NewtonMeshCreateFromCollision(collision);
		NewtonMeshApplyBoxMapping(collisionMesh, 0, 0, 0);

		// allocate the vertex data
		int vertexCount = NewtonMeshGetPointCount(collisionMesh);

		vbo.m_data.reserve(floatOffset + vertexCount * vertexSize);
		vbo.m_data.resize(floatOffset + vertexCount * vertexSize);

		NewtonMeshGetVertexStreams(collisionMesh,
				byteSize, &vbo.m_data[floatOffset + 2 + 3],
				byteSize, &vbo.m_data[floatOffset + 2],
				byteSize, &vbo.m_data[floatOffset],
				byteSize, &vbo.m_data[floatOffset]);

		// iterate over the submeshes and store the indices
		void* const meshCookie = NewtonMeshBeginHandle(collisionMesh);
		for (int handle = NewtonMeshFirstMaterial(collisionMesh, meshCookie);
				handle != -1; handle = NewtonMeshNextMaterial(collisionMesh, meshCookie, handle)) {

			// create a new submesh
			ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
			subBuffer->material = "";
			subBuffer->userData = NULL;

			subBuffer->dataCount = vertexCount;
			subBuffer->dataOffset = vertexOffset;

			// get the indices
			subBuffer->indexCount = NewtonMeshMaterialGetIndexCount(collisionMesh, meshCookie, handle);
			uint32_t* indices = new uint32_t[subBuffer->indexCount];
			NewtonMeshMaterialGetIndexStream(collisionMesh, meshCookie, handle, (int*)indices);

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
}
#endif

Domino __Domino::createDomino(Type type, const Mat4f& matrix, float mass, const std::string& material, bool doPlacement)
{
	const NewtonWorld* world = Simulation::instance().getWorld();
	const float VERTICAL_DELTA = 0.01f;
	const int materialID = MaterialMgr::instance().getID(material);

	type = std::min(type, DOMINO_LARGE);
	Vec3f size = s_domino_size[type];
	Vec3f sz = size * 0.5f;

	Mat4f mat(matrix);

	if (doPlacement) {
		Vec3f p0 = mat.getW();
		mat.setW(Vec3f());

		// generate points
		Vec3f p[4][2];
		p[0][0] = p[0][1] = p0 + Vec3f(sz.x, 0.0f, sz.z) * mat;
		p[0][0].y = newton::getVerticalPosition(world, p[0][0].x, p[0][0].z) + VERTICAL_DELTA;
		p[1][0] = p[1][1] = p0 + Vec3f(-sz.x, 0.0f, sz.z) * mat;
		p[1][0].y = newton::getVerticalPosition(world, p[1][0].x, p[1][0].z) + VERTICAL_DELTA;
		p[2][0] = p[2][1] = p0 + Vec3f(-sz.x, 0.0f, -sz.z) * mat;
		p[2][0].y = newton::getVerticalPosition(world, p[2][0].x, p[2][0].z) + VERTICAL_DELTA;
		p[3][0] = p[3][1] = p0 + Vec3f(sz.x, 0.0f, -sz.z) * mat;
		p[3][0].y = newton::getVerticalPosition(world, p[3][0].x, p[3][0].z) + VERTICAL_DELTA;

		mat = matrix;
		Vec3f pos = mat.getW();
		pos.y = std::max(std::max(std::max(p[0][0].y, p[1][0].y), p[2][0].y), p[3][0].y) + size.y * 0.5f;
		mat.setW(pos);
	}

	Mat4f identity = Mat4f::identity();

	// getCollision(type, materialID);
	NewtonCollision* collision = NewtonCreateBox(world, size.x, size.y, size.z, materialID, identity[0]);

	Domino result = Domino(new __Domino(type, mat, material));
	result->create(collision, mass, result->m_freezeState, result->m_damping);
	NewtonReleaseCollision(world, collision);

	return result;
}

}
