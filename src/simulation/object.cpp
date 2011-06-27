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
	"domino_small",
	"domino_middle",
	"domino_large",
	"box",
	"sphere",
	"cylinder",
	"capsule",
	"cone",
	"chamfercylinder",
	"hull",
	"assembly",
	"compound",
	"environment"
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
	RigidBody result = RigidBody(new __RigidBody(__Object::CHAMFER_CYLINDER, matrix, material, freezeState, damping));

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
	xml_attribute<>* attr = node->first_attribute();
	
	//attribute type
	attr = attr->next_attribute();
	std::string type = attr->value();

	//attribute matrix
	attr = attr->next_attribute();
	Mat4f matrix = Mat4f();
	matrix.assign(attr->value());

	/* set dimensions for box */
	if ( type == TypeStr[BOX]) {
		//attribute width
		attr = attr->next_attribute();
		w = atof(attr->value());

		//attribute height
		attr = attr->next_attribute();
		h = atof(attr->value());

		//attribute depth
		attr = attr->next_attribute();
		d = atof(attr->value());

	}/* set dimensions for box END */

	/* set dimensions for sphere */
	if ( type == TypeStr[SPHERE] ) {
		//attribute x
		attr = attr->next_attribute();
		x = atof(attr->value());

		//attribute y
		attr = attr->next_attribute();
		y = atof(attr->value());

		//attribute z
		attr = attr->next_attribute();
		z = atof(attr->value());
	}/* set dimensions for sphere END */

	/* set dimensions for chamfer cylinder, cylinder, cone, capsule */
	if ( type == TypeStr[CHAMFER_CYLINDER] || type == TypeStr[CYLINDER] ||
			type == TypeStr[CONE] || type == TypeStr[CAPSULE]) {
		//attribute height
		attr = attr->next_attribute();
		h = atof(attr->value());

		//attribute radius
		attr = attr->next_attribute();
		radius = atof(attr->value());
	}/* set dimensions for chamfer cylinder, cylinder, cone, capsule END */


	//attribute freezeState
	attr = attr->next_attribute();
	freezeState = atoi(attr->value());

	//attribute damping
	attr = attr->next_attribute();
	damping = Vec4f();
	damping.assign(attr->value());

	//attribute material
	attr = attr->next_attribute();
	std::string material = attr->value();

	//attribute mass
	attr = attr->next_attribute();
	float mass = atof(attr->value());

	// find the Type for the Domino
	for(i = 0; i < 3; ++i) {
		if(!strcmp(type.c_str(), TypeStr[i])) {
			t = (Type) i;
		}
	}

	if( type == TypeStr[BOX] ) return __Object::createBox(matrix, w, h, d, mass, material, freezeState, damping);
	if( type == TypeStr[SPHERE] ) return __Object::createSphere(matrix, x, y, z, mass, material, freezeState, damping);
	if( type == TypeStr[CHAMFER_CYLINDER] ) return __Object::createChamferCylinder(matrix, radius, h, mass, material, freezeState, damping);
	if( type == TypeStr[CYLINDER] ) return __Object::createCylinder(matrix, radius, h, mass, material, freezeState, damping);
	if( type == TypeStr[CONE] ) return __Object::createCone(matrix, radius, h, mass, material, freezeState, damping);
	if( type == TypeStr[CAPSULE] ) return __Object::createCapsule(matrix, radius, h, mass, material, freezeState, damping);
	if( type == TypeStr[DOMINO_SMALL] ) return __Domino::createDomino(t, matrix, mass, material, false);
	if( type == TypeStr[DOMINO_MIDDLE] ) return __Domino::createDomino(t, matrix, mass, material, false);
	if( type == TypeStr[DOMINO_LARGE] ) return __Domino::createDomino(t, matrix, mass, material, false);

	return RigidBody();
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

	float x = scale.x;
	float y = scale.y;
	float z = scale.z;

	NewtonCollisionInfoRecord info;
	NewtonCollisionGetInfo(collision, &info);
	switch (info.m_collisionType) {
	case SERIALIZE_ID_BOX:
		if (add) {
			x += info.m_box.m_x;
			y += info.m_box.m_y;
			z += info.m_box.m_z;
		}
		scaled = NewtonCreateBox(world, x, y, z, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_SPHERE:
		if (add) {
			x += info.m_sphere.m_r0;
			y += info.m_sphere.m_r1;
			z += info.m_sphere.m_r2;
		}
		scaled = NewtonCreateSphere(world, x, y, z, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CYLINDER:
		if (add) {
			x += info.m_cylinder.m_r0;
			y += info.m_cylinder.m_height;
		}
		scaled = NewtonCreateCylinder(world, x, y, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CONE:
		if (add) {
			x += info.m_cone.m_r;
			y += info.m_cone.m_height;
		}
		scaled = NewtonCreateCone(world, x, y, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CAPSULE:
		if (add) {
			x += info.m_capsule.m_r0;
			y += info.m_capsule.m_height;
		}
		scaled = NewtonCreateCapsule(world, x, y, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CHAMFERCYLINDER:
		if (add) {
			x += info.m_chamferCylinder.m_r;
			y += info.m_chamferCylinder.m_height;
		}
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

__ConvexHull::__ConvexHull(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName, bool originalGeometry, int freezeState, const Vec4f& damping)
	: __RigidBody(CONVEX_HULL, matrix, material, freezeState, damping), m_originalGeometry(originalGeometry), m_mesh(NULL), m_fileName(fileName)
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

void __ConvexHull::save(const __ConvexHull& body , rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc)
{
	using namespace rapidxml;

	char *pFreezeState, *pDamping, *pMaterial, *pMass, *pFileName, *pOriginal;
	xml_attribute<> *attrFS, *attrDamp, *attrMat, *attrMass, *attrFileName, *attrOr;

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

	// set attribute rendering type
	pOriginal = doc->allocate_string(util::toString(body.m_originalGeometry));
	attrOr = doc->allocate_attribute("original", pOriginal);
	node->append_attribute(attrOr);

}

ConvexHull __ConvexHull::load(rapidxml::xml_node<>* node)
{
	using namespace rapidxml;

	//attribute id is set in __Object::load()
	xml_attribute<>* attr = node->first_attribute();

	//attribute type
	attr = attr->next_attribute();
	std::string type = attr->value();

	//attribute matrix
	attr = attr->next_attribute();
	Mat4f matrix = Mat4f();
	matrix.assign(attr->value());

	//attribute freezeState
	attr = attr->next_attribute();
	int freezeState = atoi(attr->value());

	//attribute damping
	attr = attr->next_attribute();
	Vec4f damping = Vec4f();
	damping.assign(attr->value());

	//attribute material
	attr = attr->next_attribute();
	std::string material = attr->value();

	//attribute mass
	attr = attr->next_attribute();
	float mass = atof(attr->value());

	//attribute filename
	attr = attr->next_attribute();
	std::string filename = attr->value();

	//attribute original
	attr = attr->next_attribute();
	int original = atoi(attr->value());

	return ConvexHull(new __ConvexHull(matrix, mass, material, filename, original, freezeState, damping));
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

__ConvexAssembly::__ConvexAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName, RenderingType renderingType, int freezeState, const Vec4f& damping)
	: __RigidBody(CONVEX_ASSEMBLY, matrix, material, freezeState, damping), m_renderingType(renderingType), m_mesh(NULL), m_fileName(fileName)
{
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

	// sort meshes by the material, this way we can combine several meshes
	// to a single buffer
	std::list<MeshEntry> sortedMeshes;
	{
		for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
			if (mesh->faces > 0) {
				sortedMeshes.push_back(MeshEntry(mesh, mesh->faceL[0].material, defaultMaterial));
			}
		}
		sortedMeshes.sort(MeshEntry::compare);
	}

	Lib3dsVector* vertices = new Lib3dsVector[vertexCount];
	Lib3dsVector* normals = NULL;
	Lib3dsTexel* uvs = NULL;
	if (m_renderingType == ORIGINAL) {
		normals = new Lib3dsVector[vertexCount];
		uvs = new Lib3dsTexel[vertexCount];
	}

	unsigned finishedFaces = 0;
	unsigned hull_vOffset = 0;
	unsigned buffer_vOffset = 0;
	Mat4f identity = Mat4f::identity();

	std::vector<NewtonCollision*> collisions;

	if (m_renderingType == MESH_EXACT) {
		m_mesh = NewtonMeshCreate(world);
		NewtonMeshBeginFace(m_mesh);
	}

	for (std::list<MeshEntry>::iterator itr = sortedMeshes.begin(); itr != sortedMeshes.end(); ++itr) {
		Lib3dsMesh* mesh = itr->mesh;

		unsigned hull_vCount = 0;
		int faceMaterial = itr->materialID; //defaultMaterial;

		if (m_renderingType == ORIGINAL)
			lib3ds_mesh_calculate_normals(mesh, &normals[finishedFaces*3]);

		for(unsigned cur_face = 0; cur_face < mesh->faces; cur_face++) {
			Lib3dsFace* face = &mesh->faceL[cur_face];

			for(unsigned int i = 0; i < 3; i++) {
				memcpy(&vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
				if (mesh->texelL && m_renderingType == ORIGINAL)
					memcpy(&uvs[finishedFaces*3 + i], mesh->texelL[face->points[i]], sizeof(Lib3dsTexel));
			}

			if (m_renderingType == MESH_EXACT) {
				//faceMaterial = face->material && face->material[0] ? MaterialMgr::instance().getID(face->material) : defaultMaterial;
				NewtonMeshAddFace(m_mesh, 3, vertices[finishedFaces*3], sizeof(Lib3dsVector), faceMaterial);
			}

			finishedFaces++;
		}
		hull_vCount += mesh->faces * 3;

		// create a new sub-buffer with the mesh material
		if (m_renderingType == ORIGINAL) {
			std::list<MeshEntry>::iterator next = itr;
			++next;
			// only create it if the next mesh has another material, or this is the last mesh
			// otherwise combine it with the next mesh
			if (next == sortedMeshes.end() || next->materialID != faceMaterial) {
				ogl::SubBuffer* buffer = new ogl::SubBuffer();
				buffer->object = this;
				Material* material = MaterialMgr::instance().fromID(faceMaterial);
				buffer->material = material ? material->name : "";

				buffer->dataCount = finishedFaces*3 - buffer_vOffset;
				buffer->dataOffset = buffer_vOffset;
				buffer->indexCount = buffer->dataCount;
				buffer->indexOffset = buffer->dataOffset;
				buffer_vOffset = finishedFaces*3;
				m_buffers.push_back(buffer);
			}
		}

		collisions.push_back(NewtonCreateConvexHull(world, hull_vCount, vertices[hull_vOffset], sizeof(Lib3dsVector), 0.002f, faceMaterial, identity[0]));
		hull_vOffset += hull_vCount;
	}
	lib3ds_file_free(file);
	if (m_renderingType == MESH_EXACT)
		NewtonMeshEndFace(m_mesh);

	// create buffer data
	if (m_renderingType == ORIGINAL) {
		m_data.reserve(vertexCount * (3+3+2));
		m_data.resize(vertexCount * (3+3+2));
		for (int i = 0; i < vertexCount; ++i) {
			memcpy(&m_data[i * (3+3+2) + 0], uvs[i], 2 * sizeof(float));
			memcpy(&m_data[i * (3+3+2) + 2], normals[i], 3 * sizeof(float));
			memcpy(&m_data[i * (3+3+2) + 5], vertices[i], 3 * sizeof(float));
		}

		m_indices.reserve(vertexCount);
		for (int i = 0; i < vertexCount; ++i)
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

void __ConvexAssembly::save(const __ConvexAssembly& body , rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc)
{
	using namespace rapidxml;

	char *pFreezeState, *pDamping, *pMaterial, *pMass, *pFileName, *pRendering;
	xml_attribute<> *attrFS, *attrDamp, *attrMat, *attrMass, *attrFileName, *attrRe;

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

	// set attribute rendering type
	pRendering = doc->allocate_string(util::toString(body.m_renderingType));
	attrRe = doc->allocate_attribute("rendering", pRendering);
	node->append_attribute(attrRe);
}

ConvexAssembly __ConvexAssembly::load(rapidxml::xml_node<>* node)
{
	using namespace rapidxml;

	//attribute id is set in __Object::load()
	xml_attribute<>* attr = node->first_attribute();

	//attribute type
	attr = attr->next_attribute();
	std::string type = attr->value();

	//attribute matrix
	attr = attr->next_attribute();
	Mat4f matrix = Mat4f();
	matrix.assign(attr->value());

	//attribute freezeState
	attr = attr->next_attribute();
	int freezeState = atoi(attr->value());

	//attribute damping
	attr = attr->next_attribute();
	Vec4f damping = Vec4f();
	damping.assign(attr->value());

	//attribute material
	attr = attr->next_attribute();
	std::string material = attr->value();

	//attribute mass
	attr = attr->next_attribute();
	float mass = atof(attr->value());

	//attribute filename
	attr = attr->next_attribute();
	std::string filename = attr->value();

	//attribute rendering type
	attr = attr->next_attribute();
	int rendering = atoi(attr->value());

	return ConvexAssembly(new __ConvexAssembly(matrix, mass, material, filename, (RenderingType)rendering, freezeState, damping));
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
		/// @todo we can combine sub-meshes with the same material
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
