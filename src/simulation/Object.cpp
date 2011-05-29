/*
 * Object.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Object.hpp>
#include <simulation/Material.hpp>
#include <simulation/Simulation.hpp>
#include <newton/util.hpp>

namespace sim {

__Object::__Object(Type type)
	: m_type(type)
{
	// TODO Auto-generated constructor stub

}

__Object::~__Object()
{
	// TODO Auto-generated destructor stub
}

RigidBody __Object::createSphere(Mat4f matrix, float radius, float mass, const std::string& material)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::SPHERE, matrix, material));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateSphere(world, radius, radius, radius, materialID, identity[0]);

	result->create(collision, mass);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __Object::createBox(Mat4f matrix, float w, float h, float d, float mass, const std::string& material)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::BOX, matrix, material));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateBox(world, w, h, d, materialID, identity[0]);

	result->create(collision, mass);
	NewtonReleaseCollision(world, collision);

	return result;
}


__RigidBody::__RigidBody(Type type, NewtonBody* body, const std::string& material)
	: __Object(type), Body(body), m_material(material)

{
}

__RigidBody::__RigidBody(Type type, const Mat4f& matrix, const std::string& material)
	: __Object(type), Body(matrix), m_material(material)
{
}

__RigidBody::__RigidBody(Type type, NewtonBody* body, const Mat4f& matrix, const std::string& material)
	: __Object(type), Body(body, matrix), m_material(material)

{
}

void __RigidBody::genBuffers(ogl::VertexBuffer& vbo)
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
//	case CYLINDER:
//		NewtonMeshApplyCylindricalMapping(collisionMesh, 0, 0);
//		break;
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

	// iterate over the submeshes and store the indices
	void* const meshCookie = NewtonMeshBeginHandle(collisionMesh);
	for (int handle = NewtonMeshFirstMaterial(collisionMesh, meshCookie);
			handle != -1; handle = NewtonMeshNextMaterial(collisionMesh, meshCookie, handle)) {

		//int material = NewtonMeshMaterialGetMaterial(collisionMesh, meshCookie, handle);

		// create a new submesh
		ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
		subBuffer->material = m_material;
		subBuffer->object = this;

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

bool __RigidBody::contains(const NewtonBody* const body)
{
	return m_body == body;
}


void __RigidBody::render()
{
	newton::showCollisionShape(getCollision(), m_matrix);
}




}
