/**
 * @author Markus Doellinger, Robert Waury
 * @date May 27, 2011
 * @file simulation/object.cpp
 */

#include <simulation/object.hpp>
#include <simulation/compound.hpp>
#include <simulation/material.hpp>
#include <simulation/domino.hpp>
#include <newton/util.hpp>
#include <iostream>
#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>
#include <lib3ds/types.h>
#include <stdio.h>
#include <util/tostring.hpp>
#include <boost/foreach.hpp>
#include <locale>

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

		__Convex::save((__Convex&)object, node, doc);
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
	else return __RigidBody::load(node);
}



RigidBody __RigidBody::createSphere(const Mat4f& matrix, float radius_x, float radius_y, float radius_z, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::SPHERE, matrix, material, freezeState, damping));

	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateSphere(newton::world, radius_x, radius_y, radius_z, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(newton::world, collision);

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

	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateBox(newton::world, w, h, d, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(newton::world, collision);

	return result;
}

RigidBody __RigidBody::createBox(const Vec3f& position, float w, float h, float d, float mass, const std::string& material)
{
	return createBox(Mat4f::translate(position), w, h, d, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createCylinder(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::CYLINDER, matrix, material, freezeState, damping));

	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateCylinder(newton::world, radius, height, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(newton::world, collision);

	return result;
}

RigidBody __RigidBody::createCylinder(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createCylinder(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createChamferCylinder(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::CHAMFER_CYLINDER, matrix, material, freezeState, damping));

	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateChamferCylinder(newton::world, radius, height, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(newton::world, collision);

	return result;
}

RigidBody __RigidBody::createChamferCylinder(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createChamferCylinder(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createCapsule(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::CAPSULE, matrix, material, freezeState, damping));

	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateCapsule(newton::world, radius, height, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(newton::world, collision);

	return result;
}

RigidBody __RigidBody::createCapsule(const Vec3f& position, float radius, float height, float mass, const std::string& material)
{
	return createCapsule(Mat4f::translate(position), radius, height, mass, material, 0, Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
}

RigidBody __RigidBody::createCone(const Mat4f& matrix, float radius, float height, float mass, const std::string& material, int freezeState, const Vec4f& damping)
{
	RigidBody result = RigidBody(new __RigidBody(__Object::CONE, matrix, material, freezeState, damping));

	int materialID = MaterialMgr::instance().getID(material);
	Mat4f identity = Mat4f::identity();
	NewtonCollision* collision = NewtonCreateCone(newton::world, radius, height, materialID, identity[0]);

	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(newton::world, collision);

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

	// this prevents that the atof functions fails on German systems
	// since they use "," as a separator for floats
	setlocale(LC_ALL,"C");

	int freezeState = 0; Vec4f damping(0.1f, 0.1f, 0.1f, 0.1f);	// all except dominos use it
	float w, h, d;	// box
	float x, y, z;	// sphere
	float radius;	// chamfer cylinder, cylinder, cone, capsule
	Type t; int i;	// domino

	std::string type;
	Mat4f matrix;
	float mass;
	std::string material;

	//attribute id is set in Simulation::load()
	//xml_attribute<>* attr = 
	
	//attribute type
	xml_attribute<>* attr = node->first_attribute("type");
	if(attr) {
	type = attr->value();
	} else throw parse_error("No \"type\" attribute in object tag found", node->name());

	if (type == TypeStr[CONVEX_ASSEMBLY]) return __Convex::load(node);
	if (type == TypeStr[CONVEX_HULL]) return __Convex::load(node);

	//attribute matrix
	attr = node->first_attribute("matrix");
	if(attr) {
	matrix = Mat4f();
	matrix.assign(attr->value());
	} else throw parse_error("No \"matrix\" attribute in object tag found", node->name());


	/* set dimensions for box */
	if ( type == TypeStr[BOX]) {
		//attribute width
		attr = node->first_attribute("width");
		if(attr) {
		w = atof(attr->value());
		} else throw parse_error("No \"width\" attribute in object tag found", node->name());


		//attribute height
		attr = node->first_attribute("height");
		if(attr) {
		h = atof(attr->value());
		} else throw parse_error("No \"height\" attribute in object tag found", node->name());


		//attribute depth
		attr = node->first_attribute("depth");
		if(attr) {
		d = atof(attr->value());
		} else throw parse_error("No \"depth\" attribute in object tag found", node->name());


	}/* set dimensions for box END */

	/* set dimensions for sphere */
	if ( type == TypeStr[SPHERE] ) {
		//attribute x
		attr = node->first_attribute("radius_x");
		if(attr) {
		x = atof(attr->value());
		} else throw parse_error("No \"radius_x\" attribute in object tag found", node->name());


		//attribute y
		attr = node->first_attribute("radius_y");
		if(attr) {
		y = atof(attr->value());
		} else throw parse_error("No \"radius_y\" attribute in object tag found", node->name());


		//attribute z
		attr = node->first_attribute("radius_z");
		if(attr) {
		z = atof(attr->value());
		} else throw parse_error("No \"radius_z\" attribute in object tag found", node->name());

	}/* set dimensions for sphere END */

	/* set dimensions for chamfer cylinder, cylinder, cone, capsule */
	if ( type == TypeStr[CHAMFER_CYLINDER] || type == TypeStr[CYLINDER] ||
			type == TypeStr[CONE] || type == TypeStr[CAPSULE]) {
		//attribute height
		attr = node->first_attribute("height");
		if(attr) {
		h = atof(attr->value());
		} else throw parse_error("No \"height\" attribute in object tag found", node->name());


		//attribute radius
		attr = node->first_attribute("radius");
		if(attr) {
		radius = atof(attr->value());
		} else throw parse_error("No \"radius\" attribute in object tag found", node->name());

	}/* set dimensions for chamfer cylinder, cylinder, cone, capsule END */


	//attribute freezeState
	attr = node->first_attribute("freezeState");
	if(attr) {
	freezeState = atoi(attr->value());
	} else throw parse_error("No \"freezeState\" attribute in object tag found", node->name());


	//attribute damping
	attr = node->first_attribute("damping");
	if(attr) {
	damping = Vec4f();
	damping.assign(attr->value());
	} else throw parse_error("No \"damping\" attribute in object tag found", node->name());


	//attribute material
	attr = node->first_attribute("material");
	if(attr) {
	material = attr->value();
	} else throw parse_error("No \"material\" attribute in object tag found", node->name());


	//attribute mass
	attr = node->first_attribute("mass");
	if(attr) {
	mass = atof(attr->value());
	} else throw parse_error("No \"mass\" attribute in object tag found", node->name());


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
		scaled = NewtonCreateBox(newton::world, x, y, z, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_SPHERE:
		scaled = NewtonCreateSphere(newton::world, x, y, z, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CYLINDER:
		scaled = NewtonCreateCylinder(newton::world, x, y, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CONE:
		scaled = NewtonCreateCone(newton::world, x, y, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CAPSULE:
		scaled = NewtonCreateCapsule(newton::world, x, y, info.m_collisionUserID, NULL);
		break;
	case SERIALIZE_ID_CHAMFERCYLINDER:
		scaled = NewtonCreateChamferCylinder(newton::world, x, y, info.m_collisionUserID, NULL);
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

	NewtonReleaseCollision(newton::world, scaled);
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

__Convex::__Convex(Type type, const Mat4f& matrix, float mass, const std::string& material,
		const std::string& fileName, int freezeState, const Vec4f& damping)
	: __RigidBody(type, matrix, material, freezeState, damping), m_fileName(fileName)
{
}

__Convex::~__Convex()
{
}

Convex __Convex::createHull(const Mat4f& matrix, float mass, const std::string& material,
		const std::string& fileName, int freezeState, const Vec4f& damping)
{
	Convex result(new __Convex(CONVEX_HULL, matrix, mass, material, fileName, freezeState, damping));

	// load the visual
	ogl::Mesh visual = ogl::__Mesh::load3ds(fileName, result.get());

	int materialID = MaterialMgr::instance().getID(material);

	// create a hull from the visual
	NewtonCollision* collision = NewtonCreateConvexHull(newton::world, visual->vertexCount(),
			visual->firstVertex(), visual->byteSize(), 0.002f, materialID, NULL);
	result->create(collision, mass, freezeState, damping);
	NewtonReleaseCollision(newton::world, collision);

	result->m_visual = visual;

	return result;
}

Convex __Convex::createAssembly(const Mat4f& matrix, float mass, const std::string& material, const std::string& fileName,
		int freezeState, const Vec4f& damping)
{
	Convex result(new __Convex(CONVEX_ASSEMBLY, matrix, mass, material, fileName, freezeState, damping));

	// load the visual entity and preserve the original sub-meshes
	ogl::SubBuffers buffers;
	ogl::Mesh visual = ogl::__Mesh::load3ds(fileName, result.get(), &buffers);

	int defaultMaterial = MaterialMgr::instance().getID(material);

	// for each sub-mesh, create a convex hull
	std::vector<NewtonCollision*> collisions;
	BOOST_FOREACH(ogl::SubBuffer* buf, buffers) {
		int meshMaterial = MaterialMgr::instance().getID(buf->material);
		const float* data = visual->firstVertex() + buf->dataOffset * visual->floatSize();
		collisions.push_back(NewtonCreateConvexHull(newton::world, buf->dataCount, data, visual->byteSize(), 0.002f, meshMaterial, NULL));
		delete buf;
	}

	// create a compound from all hulls
	NewtonCollision* collision = NewtonCreateCompoundCollision(newton::world, collisions.size(), &collisions[0], defaultMaterial);
	result->create(collision, mass, freezeState, damping);

	NewtonReleaseCollision(newton::world, collision);
	BOOST_FOREACH(NewtonCollision* hull, collisions)
		NewtonReleaseCollision(newton::world, hull);

	result->m_visual = visual;

	return result;
}

void __Convex::genBuffers(ogl::VertexBuffer& vbo)
{
	m_visual->genBuffers(vbo);
}


void __Convex::save(const __Convex& body , rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc)
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

Convex __Convex::load(rapidxml::xml_node<>* node)
{
	using namespace rapidxml;

	// this prevents that the atof functions fails on German systems
	// since they use "," as a separator for floats
	setlocale(LC_ALL,"C");

	Mat4f matrix = Mat4f();
	int freezeState;
	Vec4f damping = Vec4f();
	std::string material;
	float mass;
	std::string filename;

	//attribute matrix
	xml_attribute<>* attr = node->first_attribute("matrix");
	if(attr) {
	matrix.assign(attr->value());
	} else throw parse_error("No \"matrix\" attribute in object tag found", node->name());

	//attribute freezeState
	attr = node->first_attribute("freezeState");
	if(attr) {
	freezeState = atoi(attr->value());
	} else throw parse_error("No \"freezeState\" attribute in object tag found", node->name());


	//attribute damping
	attr = node->first_attribute("damping");
	if(attr) {
	damping.assign(attr->value());
	} else throw parse_error("No \"damping\" attribute in object tag found", node->name());


	//attribute material
	attr = node->first_attribute("material");
	if(attr) {
	material = attr->value();
	} else throw parse_error("No \"material\" attribute in object tag found", node->name());


	//attribute mass
	attr = node->first_attribute("mass");
	if(attr) {
	mass = atof(attr->value());
	} else throw parse_error("No \"mass\" attribute in object tag found", node->name());


	//attribute filename
	attr = node->first_attribute("filename");
	if(attr) {
	filename = attr->value();
	} else throw parse_error("No \"filename\" attribute in object tag found", node->name());


	std::string type = node->first_attribute("type")->value();
	if(!type.empty()) {
		if (type == TypeStr[CONVEX_ASSEMBLY])
			return createAssembly(matrix, mass, material, filename, freezeState, damping);
		else
			return createHull(matrix, mass, material, filename, freezeState, damping);
	} else throw parse_error("No \"type\" attribute in object tag found", node->name());

}





}
