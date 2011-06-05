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

	//TODO save convex hull, convex assembly
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



RigidBody __Object::createSphere(const Mat4f& matrix, float radius_x, float radius_y, float radius_z, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::SPHERE, matrix, material, freezeState, damping));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateSphere(world, radius_x, radius_y, radius_z, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __Object::createSphere(const Vec3f& position, float radius_x, float radius_y, float radius_z, float mass, const std::string& material)
{
	return __Object::createSphere(Mat4f::translate(position), radius_x, radius_y, radius_z, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __Object::createSphere(const Mat4f& matrix, float radius, float mass, const std::string& material)
{
	return __Object::createSphere(matrix, radius, radius, radius, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __Object::createSphere(const Vec3f& position, float radius, float mass, const std::string& material)
{
	return createSphere(Mat4f::translate(position), radius, radius, radius, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __Object::createBox(const Mat4f& matrix, float w, float h, float d, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::BOX, matrix, material, freezeState, damping));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateBox(world, w, h, d, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __Object::createBox(const Vec3f& position, float w, float h, float d, float mass, const std::string& material)
{
	return createBox(Mat4f::translate(position), w, h, d, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __Object::createCylinder(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::CYLINDER, matrix, material, freezeState, damping));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateCylinder(world, radius, height, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __Object::createCylinder(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createCylinder(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __Object::createChamferCylinder(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::CHAMFER_CYLINER, matrix, material, freezeState, damping));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateChamferCylinder(world, radius, height, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __Object::createChamferCylinder(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createChamferCylinder(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __Object::createCapsule(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::CAPSULE, matrix, material, freezeState, damping));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateCapsule(world, radius, height, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __Object::createCapsule(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createCapsule(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __Object::createCone(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::CONE, matrix, material, freezeState, damping));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateCone(world, radius, height, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __Object::createCone(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createCone(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}


__RigidBody::__RigidBody(Type type, NewtonBody* body, const std::string& material, int freezeState, const Vec4f& damping)
	: __Object(type), Body(body), m_material(material), m_freezeState(freezeState), m_damping(damping)

{
}

__RigidBody::__RigidBody(Type type, const Mat4f& matrix, const std::string& material, int freezeState, const Vec4f& damping)
	: __Object(type), Body(matrix), m_material(material), m_freezeState(freezeState), m_damping(damping)
{
}

__RigidBody::__RigidBody(Type type, NewtonBody* body, const Mat4f& matrix, const std::string& material, int freezeState, const Vec4f& damping)
	: __Object(type), Body(body, matrix), m_material(material), m_freezeState(freezeState), m_damping(damping)

{
}

float __RigidBody::convexCastPlacement(bool apply)
{
	float vertical = newton::getConvexCastPlacement(m_body);
	Mat4f matrix = m_matrix;
	matrix._42 = vertical + 0.0001f;
	if (apply)
		setMatrix(matrix);
	return matrix._42;
}

void __RigidBody::save(const __RigidBody& body /* node */)
{
	// set attribute matrix to m_matrix.str()

	NewtonCollisionInfoRecord info;
	NewtonCollision* collision = NewtonBodyGetCollision(body.m_body);
	NewtonCollisionGetInfo(collision, &info);
	//TODO save the other primitives, too: cone, cylinder, chamfercylinder, capsule
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
	// set attribute "freezeState" to m_freezeState
	// set attribute "damping" to m_damping.str()
}

RigidBody __RigidBody::load(/*node */)
{
	// mass = get mass attribute
	// material = get material attribute
	// matrix.assign(matrix string)
	// get m_freezeState
	// get damping

	// if node.type = "box"
	// 		w = node.width
	// 		h = node.height
	// 		d = node.depth
	// 		return __Object::createBox(matrix, w, h, d, mass, material, m_initialFreezeState, m_damping)

	// if node.type = "sphere"
	// 		x = node.radius_x
	// 		y = node.radius_y
	// 		z = node.radius_z
	// 		return __Object::createSphere(matrix, x, y, z, mass, material, m_initialFreezeState, m_damping)

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
		subBuffer->material = m_material;
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
	if (NewtonBodyGetSleepState(m_body))
		glColor3f(1.0f, 1.0f, 0.0f);
	else
		glColor3f(1.0f, 0.0f, 0.0f);
	newton::showCollisionShape(getCollision(), m_matrix);
}

__ConvexHull::__ConvexHull(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName, bool originalGeometry, int freezeState, const Vec4f& damping)
	: __RigidBody(CONVEX_HULL, matrix, material, freezeState, damping), m_originalGeometry(originalGeometry), m_mesh(NULL)
{
	Lib3dsFile* file = lib3ds_file_load(fileName.c_str());
	if (!file)
		return;

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);

	Lib3dsVector* vertices;
	int numFaces = 0;
	int vertexCount = 0;
	// count the faces
	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next)
		numFaces += mesh->faces;

	if (originalGeometry) {
		vertexCount = numFaces*3;
		vertices = new Lib3dsVector[vertexCount];
		Lib3dsVector* normals = new Lib3dsVector[vertexCount];
		Lib3dsTexel* uvs = new Lib3dsTexel[vertexCount];

		unsigned finishedFaces = 0;

		// foreach mesh
		for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
			lib3ds_mesh_calculate_normals(mesh, &normals[finishedFaces*3]);
			// foreach face
			for(unsigned cur_face = 0; cur_face < mesh->faces; cur_face++) {
				Lib3dsFace* face = &mesh->faceL[cur_face];
				// foreach index
				for(unsigned int i = 0;i < 3;i++) {
					memcpy(&vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
					if (mesh->texelL) memcpy(&uvs[finishedFaces*3 + i], mesh->texelL[face->points[i]], sizeof(Lib3dsTexel));
				}
				finishedFaces++;
			}
		}


		m_data.reserve(vertexCount * (3+3+2));
		m_data.resize(vertexCount * (3+3+2));
		for (int i = 0; i < vertexCount; ++i) {
			memcpy(&m_data[i * (3+3+2) + 0], uvs[i], 2 * sizeof(float));
			memcpy(&m_data[i * (3+3+2) + 2], normals[i], 3 * sizeof(float));
			memcpy(&m_data[i * (3+3+2) + 5], vertices[i], 3 * sizeof(float));
		}

		lib3ds_file_free(file);

		delete normals;
		delete uvs;
	} else {
		vertexCount = numFaces*3;
		vertices = new Lib3dsVector[numFaces * 3];

		m_mesh = NewtonMeshCreate(world);
		NewtonMeshBeginFace(m_mesh);

		unsigned finishedFaces = 0;

		// foreach mesh
		for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
			// foreach face
			for(unsigned cur_face = 0; cur_face < mesh->faces; cur_face++) {
				Lib3dsFace* face = &mesh->faceL[cur_face];
				// foreach index
				for(unsigned int i = 0;i < 3;i++) {
					memcpy(&vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
				}
				NewtonMeshAddFace(m_mesh, 3, vertices[finishedFaces*3], sizeof(Lib3dsVector), materialID);
				finishedFaces++;
			}
		}
		lib3ds_file_free(file);
		NewtonMeshEndFace(m_mesh);
	}

	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateConvexHull(world, vertexCount, vertices[0], sizeof(Lib3dsVector), 0.002f, materialID, identity[0]);
	delete vertices;
	this->create(collision, mass, m_freezeState, m_damping);
	NewtonReleaseCollision(world, collision);
}

__ConvexHull::~__ConvexHull()
{
	if (m_mesh) NewtonMeshDestroy(m_mesh);
	m_data.clear();
}

void __ConvexHull::genBuffers(ogl::VertexBuffer& vbo)
{
	if (!m_originalGeometry) {
		const unsigned vertexSize = vbo.floatSize();
		const unsigned byteSize = vbo.byteSize();
		const unsigned floatOffset = vbo.m_data.size();
		const unsigned vertexOffset = floatOffset / vertexSize;

		int shapeID = NewtonCollisionGetUserID(NewtonBodyGetCollision(m_body));
		NewtonMeshApplyBoxMapping(m_mesh, shapeID, shapeID, shapeID);

		NewtonMeshCalculateVertexNormals(m_mesh, 45.0f * 3.1416f / 180.0f);

		// allocate the vertex data
		int vertexCount = NewtonMeshGetPointCount(m_mesh);

		vbo.m_data.reserve(floatOffset + vertexCount * vertexSize);
		vbo.m_data.resize(floatOffset + vertexCount * vertexSize);

		NewtonMeshGetVertexStreams(m_mesh,
				byteSize, &vbo.m_data[floatOffset + 2 + 3],
				byteSize, &vbo.m_data[floatOffset + 2],
				byteSize, &vbo.m_data[floatOffset],
				byteSize, &vbo.m_data[floatOffset]);


		// iterate over the submeshes and store the indices
		void* const meshCookie = NewtonMeshBeginHandle(m_mesh);
		for (int handle = NewtonMeshFirstMaterial(m_mesh, meshCookie);
				handle != -1; handle = NewtonMeshNextMaterial(m_mesh, meshCookie, handle)) {

			int materialID = NewtonMeshMaterialGetMaterial(m_mesh, meshCookie, handle);

			Material* material = MaterialMgr::instance().fromID(materialID);

			// create a new submesh
			ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
			subBuffer->material = material ? material->name : m_material;
			subBuffer->object = this;

			subBuffer->dataCount = vertexCount;
			subBuffer->dataOffset = vertexOffset;

			// get the indices
			subBuffer->indexCount = NewtonMeshMaterialGetIndexCount(m_mesh, meshCookie, handle);
			uint32_t* indices = new uint32_t[subBuffer->indexCount];
			NewtonMeshMaterialGetIndexStream(m_mesh, meshCookie, handle, (int*)indices);
			subBuffer->indexOffset = vbo.m_indices.size();

			// copy the indices to the global list and add the offset
			vbo.m_indices.reserve(vbo.m_indices.size() + subBuffer->indexCount);
			for (unsigned i = 0; i < subBuffer->indexCount; ++i)
				vbo.m_indices.push_back(vertexOffset + indices[i]);

			delete indices;
			vbo.m_buffers.push_back(subBuffer);
		}
		NewtonMeshEndHandle(m_mesh, meshCookie);

		return;
	}

	// get the offset in floats and vertices
	const unsigned vertexSize = vbo.floatSize();
	//const unsigned byteSize = vbo.byteSize();
	const unsigned floatOffset = vbo.m_data.size();
	const unsigned vertexOffset = floatOffset / vertexSize;

	vbo.m_data.reserve(floatOffset + m_data.size());
	vbo.m_data.resize(floatOffset + m_data.size());

	memcpy(&vbo.m_data[floatOffset], &m_data[0], m_data.size() * sizeof(float));

	ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
	subBuffer->material = m_material;
	subBuffer->object = this;

	subBuffer->dataCount = m_data.size() / vertexSize;
	subBuffer->dataOffset = vertexOffset;
	subBuffer->indexCount = subBuffer->dataCount;
	subBuffer->indexOffset = vbo.m_indices.size();

	// copy the indices to the global list and add the offset
	vbo.m_indices.reserve(vbo.m_indices.size() + subBuffer->indexCount);
	for (unsigned i = 0; i < subBuffer->indexCount; ++i)
		vbo.m_indices.push_back(vertexOffset + i);

	vbo.m_buffers.push_back(subBuffer);
}


__ConvexAssembly::__ConvexAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName, RenderingType renderingType, int freezeState, const Vec4f& damping)
	: __RigidBody(CONVEX_ASSEMBLY, matrix, material, freezeState, damping), m_renderingType(renderingType), m_mesh(NULL)
{
	//TODO: only create mesh if necessary (renderingtype == mesh_exact
	// store original data with per-mesh material
	Lib3dsFile* file = lib3ds_file_load(fileName.c_str());
	if (!file)
		return;

	const NewtonWorld* world = Simulation::instance().getWorld();
	int defaultMaterial = MaterialMgr::instance().getID(material);
	int numFaces = 0;

	// count the faces
	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next)
		numFaces += mesh->faces;

	int vertexCount = numFaces*3;

	Lib3dsVector* vertices = new Lib3dsVector[vertexCount];
	Lib3dsVector* normals = NULL;
	Lib3dsTexel* uvs = NULL;
	if (m_renderingType == ORIGINAL) {
		normals = new Lib3dsVector[vertexCount];
		uvs = new Lib3dsTexel[vertexCount];
	}

	unsigned finishedFaces = 0;
	unsigned vOffset = 0;
	Mat4f identity = Mat4f::identity();

	std::vector<NewtonCollision*> collisions;

	if (m_renderingType == MESH_EXACT) {
		m_mesh = NewtonMeshCreate(world);
		NewtonMeshBeginFace(m_mesh);
	}

	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
		unsigned vCount = 0;
		int faceMaterial = defaultMaterial;
		if (m_renderingType == ORIGINAL)
			lib3ds_mesh_calculate_normals(mesh, &normals[finishedFaces*3]);
		for(unsigned cur_face = 0; cur_face < mesh->faces; cur_face++) {
			Lib3dsFace* face = &mesh->faceL[cur_face];

			for(unsigned int i = 0; i < 3; i++) {
				memcpy(&vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
				if (mesh->texelL && m_renderingType == ORIGINAL)
					memcpy(&uvs[finishedFaces*3 + i], mesh->texelL[face->points[i]], sizeof(Lib3dsTexel));
			}

			faceMaterial = face->material ? MaterialMgr::instance().getID(face->material) : defaultMaterial;
			if (m_renderingType == MESH_EXACT)
				NewtonMeshAddFace(m_mesh, 3, vertices[finishedFaces*3], sizeof(Lib3dsVector), faceMaterial);

			finishedFaces++;
		}
		vCount += mesh->faces * 3;

		// create a new sub-buffer with the mesh material
		if (m_renderingType == ORIGINAL) {
			ogl::SubBuffer* buffer = new ogl::SubBuffer();
			buffer->object = this;
			Material* material = MaterialMgr::instance().fromID(faceMaterial);
			buffer->material = material ? material->name : "";

			buffer->dataCount = finishedFaces*3 - vOffset;
			buffer->dataOffset = vOffset;
			buffer->indexCount = buffer->dataCount;
			buffer->indexOffset = buffer->dataOffset;

			m_buffers.push_back(buffer);
		}

		collisions.push_back(NewtonCreateConvexHull(world, vCount, vertices[vOffset], sizeof(Lib3dsVector), 0.002f, faceMaterial, identity[0]));
		vOffset += vCount;
	}
	lib3ds_file_free(file);
	if (m_renderingType == MESH_EXACT)
		NewtonMeshEndFace(m_mesh);

	// create buffer data
	if (m_renderingType == ORIGINAL) {

		// TODO: we can combine sub-meshes with the same material
		// because all sub-meshes have the same matrix. We could
		// sort the subBuffers according to the material and then
		// combine adjacent subBuffers

		m_data.reserve(vertexCount * (3+3+2));
		m_data.resize(vertexCount * (3+3+2));
		for (int i = 0; i < vertexCount; ++i) {
			memcpy(&m_data[i * (3+3+2) + 0], uvs[i], 2 * sizeof(float));
			memcpy(&m_data[i * (3+3+2) + 2], normals[i], 3 * sizeof(float));
			memcpy(&m_data[i * (3+3+2) + 5], vertices[i], 3 * sizeof(float));
		}

		m_indices.reserve(vertexCount);
		for (unsigned i = 0; i < vertexCount; ++i)
			m_indices.push_back(i);

		delete normals;
		delete uvs;
	}

	delete vertices;

	NewtonCollision* collision = NewtonCreateCompoundCollision(world, collisions.size(), &collisions[0], defaultMaterial);
	this->create(collision, mass, m_freezeState, m_damping);

	NewtonReleaseCollision(world, collision);
	for (std::vector<NewtonCollision*>::iterator itr = collisions.begin(); itr != collisions.end(); ++itr)
		NewtonReleaseCollision(world, *itr);
}

__ConvexAssembly::~__ConvexAssembly()
{
	if (m_mesh) NewtonMeshDestroy(m_mesh);
	m_data.clear();
	for (ogl::SubBuffers::iterator itr = m_buffers.begin(); itr != m_buffers.end(); ++itr)
		delete *itr;
	m_buffers.clear();
	m_indices.clear();
}

void __ConvexAssembly::genBuffers(ogl::VertexBuffer& vbo)
{
	switch (m_renderingType) {
	case MESH_EXACT:
	{
		// get the offset in floats and vertices
		const unsigned vertexSize = vbo.floatSize();
		const unsigned byteSize = vbo.byteSize();
		const unsigned floatOffset = vbo.m_data.size();
		const unsigned vertexOffset = floatOffset / vertexSize;

		NewtonMeshApplyBoxMapping(m_mesh, 0, 0, 0);
		NewtonMeshCalculateVertexNormals(m_mesh, 45.0f * 3.1416f/180.0f);

		// allocate the vertex data
		int vertexCount = NewtonMeshGetPointCount(m_mesh);

		vbo.m_data.reserve(floatOffset + vertexCount * vertexSize);
		vbo.m_data.resize(floatOffset + vertexCount * vertexSize);

		NewtonMeshGetVertexStreams(m_mesh,
			byteSize, &vbo.m_data[floatOffset + 2 + 3],
			byteSize, &vbo.m_data[floatOffset + 2],
			byteSize, &vbo.m_data[floatOffset],
			byteSize, &vbo.m_data[floatOffset]);

		// iterate over the submeshes and store the indices
		void* const meshCookie = NewtonMeshBeginHandle(m_mesh);
		for (int handle = NewtonMeshFirstMaterial(m_mesh, meshCookie);
			handle != -1; handle = NewtonMeshNextMaterial(m_mesh, meshCookie, handle)) {

			int materialID = NewtonMeshMaterialGetMaterial(m_mesh, meshCookie, handle);
			Material* material = MaterialMgr::instance().fromID(materialID);

			// create a new submesh
			ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
			subBuffer->material = material ? material->name : m_material;
			subBuffer->object = this;

			subBuffer->dataCount = vertexCount;
			subBuffer->dataOffset = vertexOffset;

			// get the indices
			subBuffer->indexCount = NewtonMeshMaterialGetIndexCount(m_mesh, meshCookie, handle);
			uint32_t* indices = new uint32_t[subBuffer->indexCount];
			NewtonMeshMaterialGetIndexStream(m_mesh, meshCookie, handle, (int*)indices);
			subBuffer->indexOffset = vbo.m_indices.size();

			// copy the indices to the global list and add the offset
			vbo.m_indices.reserve(vbo.m_indices.size() + subBuffer->indexCount);
			for (unsigned i = 0; i < subBuffer->indexCount; ++i)
				vbo.m_indices.push_back(vertexOffset + indices[i]);

			delete indices;
			vbo.m_buffers.push_back(subBuffer);
		}
		NewtonMeshEndHandle(m_mesh, meshCookie);
		break;
	}
	case MESH_ASSEMBLY:
	{
		// TODO: we can combine sub-meshes with the same material
		// because all sub-meshes have the same matrix. We could
		// sort the collisions according to the shapeIDs and then
		// combine adjacent sub-meshes

		// foreach convex hull in the compound, create a mesh
		NewtonCollisionInfoRecord info;
		NewtonCollisionGetInfo(NewtonBodyGetCollision(m_body), &info);
		if (info.m_collisionType != SERIALIZE_ID_COMPOUND)
			return __RigidBody::genBuffers(vbo);

		for (int i = 0; i < info.m_compoundCollision.m_chidrenCount; ++i) {

			const unsigned vertexSize = vbo.floatSize();
			const unsigned byteSize = vbo.byteSize();
			const unsigned floatOffset = vbo.m_data.size();
			const unsigned vertexOffset = floatOffset / vertexSize;

			NewtonCollision* collision = info.m_compoundCollision.m_chidren[i];
			NewtonMesh* collisionMesh = NewtonMeshCreateFromCollision(collision);
			int shapeID = NewtonCollisionGetUserID(collision);
			NewtonMeshApplyBoxMapping(collisionMesh, shapeID, shapeID, shapeID);

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

				int materialID = NewtonMeshMaterialGetMaterial(collisionMesh, meshCookie, handle);

				Material* material = MaterialMgr::instance().fromID(materialID);

				// create a new submesh
				ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
				subBuffer->material = material ? material->name : m_material;
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
		break;
	}
	case ORIGINAL:
	{
		// get the offset in floats and vertices
		const unsigned vertexSize = vbo.floatSize();
		//const unsigned byteSize = vbo.byteSize();
		const unsigned floatOffset = vbo.m_data.size();
		const unsigned vertexOffset = floatOffset / vertexSize;

		vbo.m_data.reserve(floatOffset + m_data.size());
		vbo.m_data.resize(floatOffset + m_data.size());

		memcpy(&vbo.m_data[floatOffset], &m_data[0], m_data.size() * sizeof(float));

		for (ogl::SubBuffers::iterator itr = m_buffers.begin(); itr != m_buffers.end(); ++itr) {
			// todo align offsets
			ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
			subBuffer->material = (*itr)->material;
			subBuffer->object = (*itr)->object;

			subBuffer->dataCount = (*itr)->dataCount;
			subBuffer->dataOffset = (*itr)->dataOffset + vertexOffset;
			subBuffer->indexCount = (*itr)->indexCount;
			subBuffer->indexOffset = (*itr)->indexOffset + vbo.m_indices.size();

			vbo.m_buffers.push_back(subBuffer);
		}

		// copy the indices to the global list and add the offset
		vbo.m_indices.reserve(vbo.m_indices.size() + m_indices.size());
		for (unsigned i = 0; i < m_indices.size(); ++i)
			vbo.m_indices.push_back(vertexOffset + m_indices[i]);

		break;
	}
	}
}

}
