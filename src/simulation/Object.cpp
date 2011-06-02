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
#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>
#include <lib3ds/types.h>

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


void __Object::save(const __Object& object /* node */)
{
	// create new node and append it to the given node

	// set attribute "id" to m_id

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


Object __Object::load(/* node */)
{
	// load m_id from "id"

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
		NewtonMeshApplyBoxMapping(collisionMesh, 0, 0, 0);
		break;
	default:
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

		int material = NewtonMeshMaterialGetMaterial(collisionMesh, meshCookie, handle);
		std::cout << "material " << material << std::endl;
		//std::cout << "mesh " << material << std::endl;

		// create a new submesh
		ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
		subBuffer->material = m_material;
		subBuffer->object = this;

		subBuffer->dataCount = vertexCount;
		subBuffer->dataOffset = vertexOffset;

		// get the indices
		subBuffer->indexCount = NewtonMeshMaterialGetIndexCount(collisionMesh, meshCookie, handle);
		std::cout << subBuffer->indexCount << std::endl;
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

__ConvexHull::__ConvexHull(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName)
	: __RigidBody(CONVEXHULL, matrix * Mat4f::translate(Vec3f(0.0f, 20.0f, 0.0f)), material)
{
	Lib3dsFile* file = lib3ds_file_load(fileName.c_str());
	if (!file)
		return;

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);

	int numFaces = 0;

	// count the faces
	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next)
		numFaces += mesh->faces;

	m_vertexCount = numFaces*3;
	std::cout << m_vertexCount << std::endl;
	m_vertices = new Lib3dsVector[numFaces * 3];
	m_normals = new Lib3dsVector[numFaces * 3];
	m_uvs = new Lib3dsTexel[numFaces * 3];

	unsigned finishedFaces = 0;

	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
		lib3ds_mesh_calculate_normals(mesh, &m_normals[finishedFaces*3]);
		for(unsigned cur_face = 0; cur_face < mesh->faces; cur_face++) {
			Lib3dsFace* face = &mesh->faceL[cur_face];
			for(unsigned int i = 0;i < 3;i++) {
				memcpy(&m_vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
				if (mesh->texelL)
				memcpy(&m_uvs[finishedFaces*3 + i], mesh->texelL[face->points[i]], sizeof(Lib3dsTexel));
				m_vertices[finishedFaces*3 + i][0] *= 10.0f;
				m_vertices[finishedFaces*3 + i][1] *= 10.0f;
				m_vertices[finishedFaces*3 + i][2] *= 10.0f;
			}
			finishedFaces++;
		}
		std::cout << "mesh" << std::endl;
	}
	lib3ds_file_free(file);

	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateConvexHull(world, m_vertexCount, m_vertices[0], sizeof(Lib3dsVector), 0.002f, materialID, identity[0]);
	std::cout << collision << std::endl;
	this->create(collision, mass);
	NewtonReleaseCollision(world, collision);
}



void __ConvexHull::genBuffers(ogl::VertexBuffer& vbo)
{
	// to this to get smooth normals
	//__RigidBody::genBuffers(vbo);
	//return;
	// get the offset in floats and vertices
	const unsigned vertexSize = vbo.floatSize();
	const unsigned byteSize = vbo.byteSize();
	const unsigned floatOffset = vbo.m_data.size();
	const unsigned vertexOffset = floatOffset / vertexSize;

	vbo.m_data.reserve(floatOffset + m_vertexCount * vertexSize);
	vbo.m_data.resize(floatOffset + m_vertexCount * vertexSize);

	for (int i = 0; i < m_vertexCount; ++i) {
		vbo.m_data[floatOffset + i * vertexSize + 0] = m_uvs[i][0];
		vbo.m_data[floatOffset + i * vertexSize + 1] = m_uvs[i][0];
		vbo.m_data[floatOffset + i * vertexSize + 2] = m_normals[i][0];
		vbo.m_data[floatOffset + i * vertexSize + 3] = m_normals[i][1];
		vbo.m_data[floatOffset + i * vertexSize + 4] = m_normals[i][2];
		vbo.m_data[floatOffset + i * vertexSize + 5] = m_vertices[i][0];
		vbo.m_data[floatOffset + i * vertexSize + 6] = m_vertices[i][1];
		vbo.m_data[floatOffset + i * vertexSize + 7] = m_vertices[i][2];
	}

	ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
	subBuffer->material = m_material;
	subBuffer->object = this;

	subBuffer->dataCount = m_vertexCount;
	subBuffer->dataOffset = vertexOffset;
	subBuffer->indexCount = m_vertexCount;
	subBuffer->indexOffset = vbo.m_indices.size();

	// copy the indices to the global list and add the offset
	vbo.m_indices.reserve(vbo.m_indices.size() + subBuffer->indexCount);
	for (unsigned i = 0; i < subBuffer->indexCount; ++i)
		vbo.m_indices.push_back(vertexOffset + i);

	vbo.m_buffers.push_back(subBuffer);
}


__ConvexAssembly::__ConvexAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName)
	: __RigidBody(CONVEXASSEMBLY, matrix, material)
{
	Lib3dsFile* file = lib3ds_file_load(fileName.c_str());
	if (!file)
		return;

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = 99;//MaterialMgr::instance().getID(material);

	int numFaces = 0;

	// count the faces
	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next)
		numFaces += mesh->faces;

	m_vertexCount = numFaces*3;
	m_vertices = new Lib3dsVector[numFaces * 3];
	m_normals = new Lib3dsVector[numFaces * 3];
	m_uvs = new Lib3dsTexel[numFaces * 3];

	unsigned finishedFaces = 0;
	unsigned vOffset = 0;
	Mat4f identity = Mat4f::identity();

	std::vector<NewtonCollision*> collisions;

	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
		unsigned vCount = 0;
		lib3ds_mesh_calculate_normals(mesh, &m_normals[finishedFaces*3]);
		for(unsigned cur_face = 0; cur_face < mesh->faces; cur_face++) {
			Lib3dsFace* face = &mesh->faceL[cur_face];
			for(unsigned int i = 0;i < 3; i++) {
				vCount++;
				memcpy(&m_vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
				memcpy(&m_uvs[finishedFaces*3 + i], mesh->texelL[face->points[i]], sizeof(Lib3dsTexel));
				m_vertices[finishedFaces*3 + i][0] *= 10.0f;
				m_vertices[finishedFaces*3 + i][1] *= 10.0f;
				m_vertices[finishedFaces*3 + i][2] *= 10.0f;
			}
			finishedFaces++;
		}
		std::cout << "mesh" << std::endl;
		collisions.push_back(NewtonCreateConvexHull(world, vCount, m_vertices[vOffset], sizeof(Lib3dsVector), 0.002f, materialID++, identity[0]));
		vOffset += vCount;
	}
	lib3ds_file_free(file);

	NewtonCollision* collision = NewtonCreateCompoundCollision(world, collisions.size(), &collisions[0], 55);
	this->create(collision, mass);
	NewtonReleaseCollision(world, collision);
}


}
