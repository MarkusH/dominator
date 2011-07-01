/*
 * Object.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/object.hpp>
#include <simulation/compound.hpp>
#include <simulation/material.hpp>
#include <simulation/simulation.hpp>
#include <simulation/domino.hpp>
#include <newton/util.hpp>
#include <iostream>
#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>
#include <lib3ds/types.h>
#include <stdio.h>
#include <util/tostring.hpp>

namespace sim {

/*
 * only append strings for new Objects in TypeStr!!!
 * prepending Object strings will break code in __RigidBody::load()
 */
const char* __Object::TypeStr[] = {
	"domino_small", "domino_middle", "domino_large",
	"box", "sphere", "cylinder",
	"capsule", "cone", "chamfercylinder",
	"hull", "assembly", "compound",
	"environment"
};
const char* __Object::TypeName[] = {
		"Domino (small)", "Domino (middle)", "Domino (large)",
		"Box", "Sphere", "Cylinder",
		"Capsule", "Cone", "Chamfer Cylinder",
		"Hull", "Assembly", "Compound",
		"Environment"
	};
const float __Object::TypeMass[] = {
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f
	};
const bool __Object::TypeFreezeState[] = {
		true, true, true,
		false, false, false,
		false, false, false,
		false, false, false,
		false
	};
const Vec3f __Object::TypeSize[] = {
		Vec3f(), Vec3f(), Vec3f(),
		Vec3f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 2.0f, 1.0f),
		Vec3f(1.0f, 6.0f, 1.0f), Vec3f(1.0f, 2.0f, 1.0f), Vec3f(5.0f, 1.0f, 1.0f),
		Vec3f(), Vec3f(), Vec3f(),
		Vec3f()
	};

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


void __Object::save(__Object& object, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc)
{
	using namespace rapidxml;

	// declarations
	xml_node<>* node;
	char *pId, *pType, *pMatrix;
	xml_attribute<> *attrI, *attrT, *attrM;

	switch (object.m_type) {
	case DOMINO_SMALL:
	case DOMINO_MIDDLE:
	case DOMINO_LARGE:
	case BOX:
	case SPHERE:
	case CYLINDER:
	case CAPSULE:
	case CONE:
	case CHAMFER_CYLINDER:
		// create and append object node
		node = doc->allocate_node(node_element, "object");
		parent->insert_node(0, node);

		// create object attributes
		// set attribute "id" to m_id
		pId = doc->allocate_string(util::toString(object.getID()));
		attrI = doc->allocate_attribute("id", pId);
		node->append_attribute(attrI);

		// set attribute "type" to  the correct type string
		pType = doc->allocate_string(TypeStr[object.m_type]);
		attrT = doc->allocate_attribute("type", pType);
		node->append_attribute(attrT);

		// set attribute "matrix"
		pMatrix = doc->allocate_string(util::toString(object.getMatrix()));
		attrM = doc->allocate_attribute("matrix", pMatrix);
		node->append_attribute(attrM);

		// load body data
		__RigidBody::save((__RigidBody&) object, node, doc);

		break;
	case CONVEX_ASSEMBLY:
	case CONVEX_HULL:
		// create and append object node
		node = doc->allocate_node(node_element, "object");
		parent->insert_node(0, node);

		// create object attributes
		// set attribute "id" to m_id
		pId = doc->allocate_string(util::toString(object.getID()));
		attrI = doc->allocate_attribute("id", pId);
		node->append_attribute(attrI);

		// set attribute "type" to  the correct type string
		pType = doc->allocate_string(TypeStr[object.m_type]);
		attrT = doc->allocate_attribute("type", pType);
		node->append_attribute(attrT);

		// set attribute "matrix"
		pMatrix = doc->allocate_string(util::toString(object.getMatrix()));
		attrM = doc->allocate_attribute("matrix", pMatrix);
		node->append_attribute(attrM);

		if (object.m_type == CONVEX_ASSEMBLY)
			__ConvexAssembly::save((__ConvexAssembly&)object, node, doc);
		else
			__ConvexHull::save((__ConvexHull&)object, node, doc);

		break;
	case COMPOUND:
		node = doc->allocate_node(node_element, "compound");
		parent->insert_node(0, node);

		// create object attributes
		// set attribute "id" to m_id
		pId = doc->allocate_string(util::toString(object.getID()));
		attrI = doc->allocate_attribute("id", pId);
		node->append_attribute(attrI);

		// set attribute "matrix"
		pMatrix = doc->allocate_string(util::toString(object.getMatrix()));
		attrM = doc->allocate_attribute("matrix", pMatrix);
		node->append_attribute(attrM);
		
		// load compound members
		__Compound::save((__Compound&)object, node, doc);

		break;
	default:
		return;
	}
}


Object __Object::load(rapidxml::xml_node<>* node)
{
	if (std::string(node->name()) == TypeStr[COMPOUND])	return __Compound::load(node);
	else if (std::string(node->name()) == TypeStr[DOMINO_SMALL]) return __Domino::load(node);
	else if (std::string(node->name()) == TypeStr[DOMINO_MIDDLE]) return __Domino::load(node);
	else if (std::string(node->name()) == TypeStr[DOMINO_LARGE]) return __Domino::load(node);
	else if (std::string(node->name()) == TypeStr[CONVEX_ASSEMBLY]) return __ConvexAssembly::load(node);
	else if (std::string(node->name()) == TypeStr[CONVEX_HULL]) return __ConvexHull::load(node);
	else return __RigidBody::load(node);
}



RigidBody __RigidBody::createSphere(const Mat4f& matrix, float radius_x, float radius_y, float radius_z, float mass, const std::string& material, int freezeState, const Vec4f& damping)
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

RigidBody __RigidBody::createSphere(const Vec3f& position, float radius_x, float radius_y, float radius_z, float mass, const std::string& material)
{
	return createSphere(Mat4f::translate(position), radius_x, radius_y, radius_z, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createSphere(const Mat4f& matrix, float radius, float mass, const std::string& material)
{
	return createSphere(matrix, radius, radius, radius, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createSphere(const Vec3f& position, float radius, float mass, const std::string& material)
{
	return createSphere(Mat4f::translate(position), radius, radius, radius, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createBox(const Mat4f& matrix, float w, float h, float d, float mass, const std::string& material, int freezeState, const Vec4f& damping)
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

RigidBody __RigidBody::createBox(const Vec3f& position, float w, float h, float d, float mass, const std::string& material)
{
	return createBox(Mat4f::translate(position), w, h, d, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createCylinder(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
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

RigidBody __RigidBody::createCylinder(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createCylinder(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createChamferCylinder(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::CHAMFER_CYLINDER, matrix, material, freezeState, damping));

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateChamferCylinder(world, radius, height, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(world, collision);

	return result;
}

RigidBody __RigidBody::createChamferCylinder(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createChamferCylinder(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createCapsule(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
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

RigidBody __RigidBody::createCapsule(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createCapsule(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createCone(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
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

RigidBody __RigidBody::createCone(const Vec3f& position, float radius, float height, float mass, const std::string& material)
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

float __RigidBody::convexCastPlacement(bool apply, std::list<NewtonBody*>* noCollision)
{
	float vertical = newton::getConvexCastPlacement(m_body, noCollision);
	Mat4f matrix = m_matrix;
	matrix._42 = vertical + 0.0001f;
	if (apply)
		setMatrix(matrix);
	return matrix._42;
}

void __RigidBody::save(const __RigidBody& body, rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc)
{
	using namespace rapidxml;

	char *pMaterial, *pMass;
	xml_attribute<> *attrMat, *attrMass;

	// dominos don't use these
	char *pFreezeState, *pDamping;
	xml_attribute<> *attrFS, *attrDamp;

	// sphere only
	char *pRadiusX, *pRadiusY, *pRadiusZ;
	xml_attribute<> *attrX, *attrY, *attrZ;

	// box only
	char *pWidth, *pHeight, *pDepth;
	xml_attribute<> *attrW, *attrH, *attrD;

	// chamfer cylinder, cylinder, cone, capsule
	char *pRadius;
	xml_attribute<> *attrR;

	NewtonCollisionInfoRecord info;
	NewtonCollision* collision = NewtonBodyGetCollision(body.m_body);
	NewtonCollisionGetInfo(collision, &info);

	if(body.m_type != DOMINO_SMALL && body.m_type != DOMINO_MIDDLE && body.m_type != DOMINO_LARGE) {
	switch (info.m_collisionType) {
	case SERIALIZE_ID_BOX:
		// set attribute width
		pWidth = doc->allocate_string(util::toString(info.m_box.m_x));
		attrW = doc->allocate_attribute("width", pWidth);
		node->append_attribute(attrW);
		// set attribute height
		pHeight = doc->allocate_string(util::toString(info.m_box.m_y));
		attrH = doc->allocate_attribute("height", pHeight);
		node->append_attribute(attrH);
		// set attribute depth
		pDepth = doc->allocate_string(util::toString(info.m_box.m_z));
		attrD = doc->allocate_attribute("depth", pDepth);
		node->append_attribute(attrD);
		break;
	case SERIALIZE_ID_SPHERE:
		// set attribute radius_x
		pRadiusX = doc->allocate_string(util::toString(info.m_sphere.m_r0));
		attrX = doc->allocate_attribute("radius_x", pRadiusX);
		node->append_attribute(attrX);
		// set attribute radius_y
		pRadiusY = doc->allocate_string(util::toString(info.m_sphere.m_r1));
		attrY = doc->allocate_attribute("radius_y", pRadiusY);
		node->append_attribute(attrY);
		// set attribute radius_z
		pRadiusZ = doc->allocate_string(util::toString(info.m_sphere.m_r2));
		attrZ = doc->allocate_attribute("radius_z", pRadiusZ);
		node->append_attribute(attrZ);
		break;
	case SERIALIZE_ID_CHAMFERCYLINDER:
		// set attribute height
		pHeight = doc->allocate_string(util::toString(info.m_chamferCylinder.m_height));
		attrH = doc->allocate_attribute("height", pHeight);
		node->append_attribute(attrH);
		// set attribute radius
		pRadius = doc->allocate_string(util::toString(info.m_chamferCylinder.m_r));
		attrR = doc->allocate_attribute("radius", pRadius);
		node->append_attribute(attrR);
		break;
	case SERIALIZE_ID_CYLINDER:
		// set attribute height
		pHeight = doc->allocate_string(util::toString(info.m_cylinder.m_height));
		attrH = doc->allocate_attribute("height", pHeight);
		node->append_attribute(attrH);
		// set attribute radius
		pRadius = doc->allocate_string(util::toString(info.m_cylinder.m_r0));
		attrR = doc->allocate_attribute("radius", pRadius);
		node->append_attribute(attrR);
		break;
	case SERIALIZE_ID_CONE:
		// set attribute height
		pHeight = doc->allocate_string(util::toString(info.m_cone.m_height));
		attrH = doc->allocate_attribute("height", pHeight);
		node->append_attribute(attrH);
		// set attribute radius
		pRadius = doc->allocate_string(util::toString(info.m_cone.m_r));
		attrR = doc->allocate_attribute("radius", pRadius);
		node->append_attribute(attrR);
		break;
	case SERIALIZE_ID_CAPSULE:
		// set attribute height
		pHeight = doc->allocate_string(util::toString(info.m_capsule.m_height));
		attrH = doc->allocate_attribute("height", pHeight);
		node->append_attribute(attrH);
		// set attribute radius
		pRadius = doc->allocate_string(util::toString(info.m_capsule.m_r0));
		attrR = doc->allocate_attribute("radius", pRadius);
		node->append_attribute(attrR);
		break;
	}
	}

	// set attribute freezeState
	pFreezeState = doc->allocate_string(util::toString(body.m_freezeState));
	attrFS = doc->allocate_attribute("freezeState", pFreezeState);
	node->append_attribute(attrFS);

	// set attribute damping
	pDamping = doc->allocate_string(util::toString(body.m_damping));
	attrDamp = doc->allocate_attribute("damping", pDamping);
	node->append_attribute(attrDamp);

	// set attribute material
	pMaterial = doc->allocate_string(body.m_material.c_str());
	attrMat = doc->allocate_attribute("material", pMaterial);
	node->append_attribute(attrMat);
	
	// set attribute mass
	pMass = doc->allocate_string(util::toString(body.getMass()));
	attrMass = doc->allocate_attribute("mass", pMass);
	node->append_attribute(attrMass);
}

RigidBody __RigidBody::load(rapidxml::xml_node<>* node)
{
	using namespace rapidxml;

	int freezeState = 0; Vec4f damping(0.1f, 0.1f, 0.1f, 0.1f);	// all except dominos use it
	float w, h, d;	// box
	float x, y, z;	// sphere
	float radius;	// chamfer cylinder, cylinder, cone, capsule
	Type t; int i;	// domino

	//attribute id is set in Simulation::load()
	//xml_attribute<>* attr = 
	
	//attribute type
	xml_attribute<>* attr = node->first_attribute("type");
	std::string type = attr->value();

	//attribute matrix
	attr = node->first_attribute("matrix");
	Mat4f matrix = Mat4f();
	matrix.assign(attr->value());

	/* set dimensions for box */
	if ( type == TypeStr[BOX]) {
		//attribute width
		attr = node->first_attribute("width");
		w = atof(attr->value());

		//attribute height
		attr = node->first_attribute("height");
		h = atof(attr->value());

		//attribute depth
		attr = node->first_attribute("depth");
		d = atof(attr->value());

	}/* set dimensions for box END */

	/* set dimensions for sphere */
	if ( type == TypeStr[SPHERE] ) {
		//attribute x
		attr = node->first_attribute("radius_x");
		x = atof(attr->value());

		//attribute y
		attr = node->first_attribute("radius_y");
		y = atof(attr->value());

		//attribute z
		attr = node->first_attribute("radius_z");
		z = atof(attr->value());
	}/* set dimensions for sphere END */

	/* set dimensions for chamfer cylinder, cylinder, cone, capsule */
	if ( type == TypeStr[CHAMFER_CYLINDER] || type == TypeStr[CYLINDER] ||
			type == TypeStr[CONE] || type == TypeStr[CAPSULE]) {
		//attribute height
		attr = node->first_attribute("height");
		h = atof(attr->value());

		//attribute radius
		attr = node->first_attribute("radius");
		radius = atof(attr->value());
	}/* set dimensions for chamfer cylinder, cylinder, cone, capsule END */


	//attribute freezeState
	attr = node->first_attribute("freezeState");
	freezeState = atoi(attr->value());

	//attribute damping
	attr = node->first_attribute("damping");
	damping = Vec4f();
	damping.assign(attr->value());

	//attribute material
	attr = node->first_attribute("material");
	std::string material = attr->value();

	//attribute mass
	attr = node->first_attribute("mass");
	float mass = atof(attr->value());

	// find the Type for the Domino
	for(i = 0; i < 3; ++i) {
		if(!strcmp(type.c_str(), TypeStr[i])) {
			t = (Type) i;
		}
	}

	RigidBody result;

	if( type == TypeStr[BOX] ) result = __RigidBody::createBox(matrix, w, h, d, mass, material, freezeState, damping);
	if( type == TypeStr[SPHERE] ) result = __RigidBody::createSphere(matrix, x, y, z, mass, material, freezeState, damping);
	if( type == TypeStr[CHAMFER_CYLINDER] ) result = __RigidBody::createChamferCylinder(matrix, radius, h, mass, material, freezeState, damping);
	if( type == TypeStr[CYLINDER] ) result = __RigidBody::createCylinder(matrix, radius, h, mass, material, freezeState, damping);
	if( type == TypeStr[CONE] ) result = __RigidBody::createCone(matrix, radius, h, mass, material, freezeState, damping);
	if( type == TypeStr[CAPSULE] ) result = __RigidBody::createCapsule(matrix, radius, h, mass, material, freezeState, damping);
	if( type == TypeStr[DOMINO_SMALL] ) result = __Domino::createDomino(t, matrix, mass, material, false);
	if( type == TypeStr[DOMINO_MIDDLE] ) result = __Domino::createDomino(t, matrix, mass, material, false);
	if( type == TypeStr[DOMINO_LARGE] ) result = __Domino::createDomino(t, matrix, mass, material, false);

	return result;
}

void __RigidBody::setMaterial(const std::string& material)
{
	m_material = material;
	int materialID = MaterialMgr::instance().getID(material);
	NewtonCollisionSetUserID(NewtonBodyGetCollision(m_body), materialID);
}

void __RigidBody::setMass(float mass)
{
	const NewtonCollision* collision = NewtonBodyGetCollision(m_body);

	Vec4f origin, inertia;
	NewtonConvexCollisionCalculateInertialMatrix(collision, &inertia[0], &origin[0]);

	if (mass < 0.0f)
		mass = NewtonConvexCollisionCalculateVolume(collision) * 0.5f;

	NewtonBodySetMassMatrix(m_body, mass, mass * inertia.x, mass * inertia.y, mass * inertia.z);
}

Vec3f __RigidBody::getSize()
{
	const NewtonCollision* collision = NewtonBodyGetCollision(m_body);
	NewtonCollisionInfoRecord info;
	NewtonCollisionGetInfo(collision, &info);
	switch (info.m_collisionType) {
	case SERIALIZE_ID_BOX:
		return Vec3f(info.m_box.m_x, info.m_box.m_y, info.m_box.m_z);
	case SERIALIZE_ID_SPHERE:
		return Vec3f(info.m_sphere.m_r0, info.m_sphere.m_r1, info.m_sphere.m_r2);
	case SERIALIZE_ID_CYLINDER:
		return Vec3f(info.m_cylinder.m_r0, info.m_cylinder.m_height, 0.0f);
	case SERIALIZE_ID_CONE:
		return Vec3f(info.m_cone.m_r, info.m_cone.m_height, 0.0f);
	case SERIALIZE_ID_CAPSULE:
		return Vec3f(info.m_capsule.m_r0, info.m_capsule.m_height, 0.0f);
	case SERIALIZE_ID_CHAMFERCYLINDER:
		return Vec3f(info.m_chamferCylinder.m_r, info.m_chamferCylinder.m_height, 0.0f);
	}
	return Vec3f();
}

bool __RigidBody::scale(const Vec3f& scale, bool add)
{
	const NewtonWorld* world = Simulation::instance().getWorld();
	if (m_type <= DOMINO_LARGE)
		return false;

	const NewtonCollision* collision = NewtonBodyGetCollision(m_body);
	NewtonCollision* scaled = NULL;

	Vec3f temp = add ? getSize() : Vec3f();
	float x = scale.x + temp.x;
	float y = scale.y + temp.y;
	float z = scale.z + temp.z;

	NewtonCollisionInfoRecord info;
	NewtonCollisionGetInfo(collision, &info);
	switch (info.m_collisionType) {
	case SERIALIZE_ID_BOX:
		scaled = NewtonCreateBox(world, x, y, z, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_SPHERE:
		scaled = NewtonCreateSphere(world, x, y, z, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CYLINDER:
		scaled = NewtonCreateCylinder(world, x, y, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CONE:
		scaled = NewtonCreateCone(world, x, y, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CAPSULE:
		scaled = NewtonCreateCapsule(world, x, y, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CHAMFERCYLINDER:
		scaled = NewtonCreateChamferCylinder(world, x, y, info.m_collisionUserID, NULL);
		break;
	default:
		return false;
	}
	NewtonBodySetCollision(m_body, scaled);
	Vec3f inertia, origin;
	float mass;
	NewtonBodyGetMassMatrix(m_body, &mass, &inertia.x, &inertia.y, &inertia.z);
	NewtonConvexCollisionCalculateInertialMatrix(scaled, &inertia[0], &origin[0]);

	if (mass != 0.0f) {
		mass = NewtonConvexCollisionCalculateVolume(scaled) * 0.5f;
		NewtonBodySetMassMatrix(m_body, mass, mass * inertia.x, mass * inertia.y, mass * inertia.z);
	}

	NewtonBodySetCentreOfMass(m_body, &origin[0]);

	NewtonReleaseCollision(world, scaled);
	return true;
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
	//TODO: this is not working yet
	//case CYLINDER:
	//case CHAMFER_CYLINDER:
	//	NewtonMeshApplyCylindricalMapping(collisionMesh, 0, 0);
	//	break;
	case BOX:
	default:
		NewtonMeshApplyBoxMapping(collisionMesh, 0, 0, 0);
		break;
	}

	//NewtonMeshCalculateVertexNormals(collisionMesh, 60.0f * 3.1416f/180.0f);

	// allocate the vertex data
	unsigned vertexCount = NewtonMeshGetPointCount(collisionMesh);

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
		subBuffer->material = m_material;
		subBuffer->userData = this;

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

bool __RigidBody::contains(const __Object* object)
{
	if (object == this)
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



ConvexHull __ConvexHull::createHull(const Mat4f& matrix, float mass, const std::string& material,
		const std::string& fileName, int freezeState, const Vec4f& damping)
{
	ConvexHull result(new __ConvexHull(matrix, mass, material, fileName, freezeState, damping));
	return result;
}

__ConvexHull::__ConvexHull(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName, int freezeState, const Vec4f& damping)
	: __RigidBody(CONVEX_HULL, matrix, material, freezeState, damping), m_fileName(fileName)
{
	m_visual = ogl::__Mesh::load3ds(fileName, this);

	const NewtonWorld* world = Simulation::instance().getWorld();
	int materialID = MaterialMgr::instance().getID(material);

	NewtonCollision* collision = NewtonCreateConvexHull(world, m_visual->vertexCount(), m_visual->firstVertex(), m_visual->byteSize(), 0.002f, materialID, NULL);
	this->create(collision, mass, m_freezeState, m_damping);
	NewtonReleaseCollision(world, collision);
}

void __ConvexHull::save(const __ConvexHull& body , rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc)
{
	using namespace rapidxml;

	char *pFreezeState, *pDamping, *pMaterial, *pMass, *pFileName;
	xml_attribute<> *attrFS, *attrDamp, *attrMat, *attrMass, *attrFileName;

	// set attribute freezeState
	pFreezeState = doc->allocate_string(util::toString(body.m_freezeState));
	attrFS = doc->allocate_attribute("freezeState", pFreezeState);
	node->append_attribute(attrFS);

	// set attribute damping
	pDamping = doc->allocate_string(util::toString(body.m_damping));
	attrDamp = doc->allocate_attribute("damping", pDamping);
	node->append_attribute(attrDamp);

	// set attribute material
	pMaterial = doc->allocate_string(body.m_material.c_str());
	attrMat = doc->allocate_attribute("material", pMaterial);
	node->append_attribute(attrMat);

	// set attribute mass
	pMass = doc->allocate_string(util::toString(body.getMass()));
	attrMass = doc->allocate_attribute("mass", pMass);
	node->append_attribute(attrMass);

	// set attribute filename to the correct file name
	pFileName = doc->allocate_string(body.m_fileName.c_str());
	attrFileName = doc->allocate_attribute("filename", pFileName);
	node->append_attribute(attrFileName);
}

ConvexHull __ConvexHull::load(rapidxml::xml_node<>* node)
{
	using namespace rapidxml;
	

	//attribute type
	xml_attribute<>* attr = node->first_attribute("attribute");
	std::string type = attr->value();

	//attribute matrix
	attr = node->first_attribute("matrix");
	Mat4f matrix = Mat4f();
	matrix.assign(attr->value());

	//attribute freezeState
	attr = node->first_attribute("freezeState");
	int freezeState = atoi(attr->value());

	//attribute damping
	attr = node->first_attribute("damping");
	Vec4f damping = Vec4f();
	damping.assign(attr->value());

	//attribute material
	attr = node->first_attribute("material");
	std::string material = attr->value();

	//attribute mass
	attr = node->first_attribute("mass");
	float mass = atof(attr->value());

	//attribute filename
	attr = node->first_attribute("filename");
	std::string filename = attr->value();

	return createHull(matrix, mass, material, filename, freezeState, damping);
}

__ConvexHull::~__ConvexHull()
{
}

void __ConvexHull::genBuffers(ogl::VertexBuffer& vbo)
{
	m_visual->genBuffers(vbo);
}

struct MeshEntry {
	Lib3dsMesh* mesh;
	int materialID;
	MeshEntry() { }
	MeshEntry(Lib3dsMesh* mesh, const char* material, int defaultMaterial) : mesh(mesh) {
		materialID = material && material[0] ? MaterialMgr::instance().getID(material) : defaultMaterial;
	}
	static bool compare(const MeshEntry& first, const MeshEntry& second) {
		return first.materialID < second.materialID;
	}
};

ConvexAssembly __ConvexAssembly::createAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
		int freezeState, const Vec4f& damping)
{
	ConvexAssembly result(new __ConvexAssembly(matrix, mass, material, fileName, freezeState, damping));
	return result;
}

__ConvexAssembly::__ConvexAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName, int freezeState, const Vec4f& damping)
	: __RigidBody(CONVEX_ASSEMBLY, matrix, material, freezeState, damping), m_fileName(fileName)
{
	ogl::SubBuffers buffers;
	m_visual = ogl::__Mesh::load3ds(fileName, this, &buffers);

	const NewtonWorld* world = Simulation::instance().getWorld();
	int defaultMaterial = MaterialMgr::instance().getID(material);

	std::vector<NewtonCollision*> collisions;

	for (ogl::SubBuffers::iterator itr = buffers.begin(); itr != buffers.end(); ++itr) {
		int meshMaterial = MaterialMgr::instance().getID((*itr)->material);
		collisions.push_back(NewtonCreateConvexHull(world, (*itr)->dataCount, m_visual->firstVertex() + (*itr)->dataOffset * m_visual->floatSize(),
				m_visual->byteSize(), 0.002f, meshMaterial, NULL));
		delete *itr;
	}

	NewtonCollision* collision = NewtonCreateCompoundCollision(world, collisions.size(), &collisions[0], defaultMaterial);
	this->create(collision, mass, m_freezeState, m_damping);

	NewtonReleaseCollision(world, collision);
	for (std::vector<NewtonCollision*>::iterator itr = collisions.begin(); itr != collisions.end(); ++itr)
		NewtonReleaseCollision(world, *itr);
}

void __ConvexAssembly::save(const __ConvexAssembly& body , rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc)
{
	using namespace rapidxml;

	char *pFreezeState, *pDamping, *pMaterial, *pMass, *pFileName;
	xml_attribute<> *attrFS, *attrDamp, *attrMat, *attrMass, *attrFileName;

	// set attribute freezeState
	pFreezeState = doc->allocate_string(util::toString(body.m_freezeState));
	attrFS = doc->allocate_attribute("freezeState", pFreezeState);
	node->append_attribute(attrFS);

	// set attribute damping
	pDamping = doc->allocate_string(util::toString(body.m_damping));
	attrDamp = doc->allocate_attribute("damping", pDamping);
	node->append_attribute(attrDamp);

	// set attribute material
	pMaterial = doc->allocate_string(body.m_material.c_str());
	attrMat = doc->allocate_attribute("material", pMaterial);
	node->append_attribute(attrMat);

	// set attribute mass
	pMass = doc->allocate_string(util::toString(body.getMass()));
	attrMass = doc->allocate_attribute("mass", pMass);
	node->append_attribute(attrMass);

	// set attribute filename to the correct file name
	pFileName = doc->allocate_string(body.m_fileName.c_str());
	attrFileName = doc->allocate_attribute("filename", pFileName);
	node->append_attribute(attrFileName);
}

ConvexAssembly __ConvexAssembly::load(rapidxml::xml_node<>* node)
{
	using namespace rapidxml;

	//attribute type
	xml_attribute<>* attr = node->first_attribute("type");
	std::string type = attr->value();

	//attribute matrix
	attr = node->first_attribute("matrix");
	Mat4f matrix = Mat4f();
	matrix.assign(attr->value());

	//attribute freezeState
	attr = node->first_attribute("freezeState");
	int freezeState = atoi(attr->value());

	//attribute damping
	attr = node->first_attribute("damping");
	Vec4f damping = Vec4f();
	damping.assign(attr->value());

	//attribute material
	attr = node->first_attribute("material");
	std::string material = attr->value();

	//attribute mass
	attr = node->first_attribute("mass");
	float mass = atof(attr->value());

	//attribute filename
	attr = node->first_attribute("filename");
	std::string filename = attr->value();

	return createAssembly(matrix, mass, material, filename, freezeState, damping);
}

__ConvexAssembly::~__ConvexAssembly()
{
}

void __ConvexAssembly::genBuffers(ogl::VertexBuffer& vbo)
{
	m_visual->genBuffers(vbo);
}

}
