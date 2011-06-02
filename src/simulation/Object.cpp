/*
 * Object.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Object.hpp>
#include <simulation/Compound.hpp>
#include <simulation/Material.hpp>
#include <simulation/Simulation.hpp>
#include <newton/util.hpp>
#include <iostream>

namespace sim {

__Object::__Object(Type type)
	: m_type(type)
{
	// TODO Auto-generated constructor stub

}

__Object::~__Object()
{
#ifdef _DEBUG
	std::cout << "delete object" << std::endl;
#endif
}


void __Object::save(int id, const __Object& object /* node */)
{
	// create new node and append it to the given node

	// set attribute id

	switch (object.m_type) {
	case BOX:
		// set attribute "type" to  "box"
		break;
	case SPHERE:
		// set attribute "type" to  "sphere"
		break;
	case COMPOUND:
		// set attribute "type" to  "compound"
		//__Compound::save(object, generatedNode);
		return;
	default:
		return;
	}

	__RigidBody::save((const __RigidBody&)object /*generatedNode*/);
}


Object __Object::load(int* id /* node */)
{
	// store the id in the *id parameter

	// if type == compound
	// 		return __Compound::load(node)

	return __RigidBody::load(/*node*/);
}

RigidBody __Object::createSphere(Mat4f matrix,
		float radius,
		float mass,
		const std::string& material)
{
	return __Object::createSphere(matrix,
			radius, radius, radius,
			mass, material);
}

RigidBody __Object::createSphere(Mat4f matrix,
		float radius_x,
		float radius_y,
		float radius_z,
		float mass,
		const std::string& material)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::SPHERE, matrix, material));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateSphere(world, radius_x, radius_y, radius_z, materialID, identity[0]);

	result->create(collision, mass);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __Object::createSphere(Vec3f position, float radius, float mass, const std::string& material)
{
	return createSphere(Mat4f::translate(position), radius, radius, radius, mass, material);
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

RigidBody __Object::createBox(Vec3f position, float w, float h, float d, float mass, const std::string& material)
{
	return createBox(Mat4f::translate(position), w, h, d, mass, material);
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

void __RigidBody::save(const __RigidBody& body /* node */)
{
	// set attribute matrix to m_matrix.str()

	NewtonCollisionInfoRecord info;
	NewtonCollision* collision = NewtonBodyGetCollision(body.m_body);
	NewtonCollisionGetInfo(collision, &info);
	switch (info.m_collisionType) {
	case SERIALIZE_ID_BOX:
		// set attribute "width" to info.m_box.m_x
		// set attribute "height" to info.m_box.m_y
		// set attribute "depth" to info.m_box.m_z
		break;
	case SERIALIZE_ID_SPHERE:
		// set attribute "radius_x" to info.m_sphere.m_r0
		// set attribute "radius_y" to info.m_sphere.m_r1
		// set attribute "radius_z" to info.m_sphere.m_r2
		break;
	}
	// set attribute "material" to body.m_material
	// set attribute "mass" to body.getMass()
}

RigidBody __RigidBody::load(/*node */)
{
	// mass = get mass attribute
	// material = get material attribute
	// matrix.assign(matrix string)

	// if node.type = "box"
	// 		w = node.width
	// 		h = node.height
	// 		d = node.depth
	// 		return __Object::createBox(matrix, w, h, d, mass, material)

	// if node.type = "sphere"
	// 		x = node.radius_x
	// 		y = node.radius_y
	// 		z = node.radius_z
	// 		return __Object::creaSphere(matrix, x, y, z, mass, material)

	return RigidBody();
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



	//std::cout << "____ " << std::endl;


	// iterate over the submeshes and store the indices
	void* const meshCookie = NewtonMeshBeginHandle(collisionMesh);
	for (int handle = NewtonMeshFirstMaterial(collisionMesh, meshCookie);
			handle != -1; handle = NewtonMeshNextMaterial(collisionMesh, meshCookie, handle)) {

		int material = NewtonMeshMaterialGetMaterial(collisionMesh, meshCookie, handle);
		//std::cout << "mesh " << material << std::endl;

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

bool __RigidBody::contains(const NewtonBody* const body)
{
	return m_body == body;
}

bool __RigidBody::contains(const Object& object)
{
	if (object.get() == this)
		return true;
	return false;
}


void __RigidBody::render()
{
	newton::showCollisionShape(getCollision(), m_matrix);
}




}
