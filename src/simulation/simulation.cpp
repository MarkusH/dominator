/*
 * Simulation.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <xml/rapidxml.hpp>
#include <xml/rapidxml_utils.hpp>
#include <xml/rapidxml_print.hpp>
#include <fstream>
#include <simulation/simulation.hpp>
#include <simulation/compound.hpp>
#include <simulation/treecollision.hpp>
#include <simulation/material.hpp>
#include <opengl/texture.hpp>
#include <opengl/shader.hpp>
#include <iostream>
#include <newton/util.hpp>
#include <simulation/domino.hpp>
#include <simulation/crspline.hpp>
#include <opengl/oglutil.hpp>

namespace sim {

Simulation* Simulation::s_instance = NULL;

// INT_ROTATE
static Mat4f rot_mat_start;
static Vec3f rot_drag_start;
static Vec3f rot_drag_cur;
static Vec2i rot_mouse;
static Vec3f rot_p1, rot_p2, rot_p3;

// INT_DOMINO_CURVE
static Vec3f curve_current;
static CRSpline curve_spline;

ObjectInfo::ObjectInfo(__Object::Type type, const std::string& material, const std::string& fileName, const float mass, const int freezeState, const Vec3f& size)
	: type(type), material(material), fileName(fileName), mass(mass), freezeState(freezeState), size(size)
{
	// use this to test the template
	//this->type = __Object::COMPOUND;
	//this->fileName = "data/templates/template.xml";
	switch (type) {
	case __Object::BOX:
	case __Object::SPHERE:
		this->size = Vec3f(1.0f, 1.0f, 1.0f);
		this->size = Vec3f(1.0f, 1.0f, 1.0f);
		break;
	case __Object::CYLINDER:
	case __Object::CONE:
		this->size = Vec3f(1.0f, 2.0f, 1.0f);
		break;
	case __Object::CAPSULE:
		this->size = Vec3f(1.0f, 6.0f, 1.0f);
		break;
	case __Object::CHAMFER_CYLINDER:
		this->size = Vec3f(5.0f, 1.0f, 1.0f);
		break;
	default:
		this->size = Vec3f(1.0f, 1.0f, 1.0f);
		break;
	}
}

Object ObjectInfo::create(const Mat4f& matrix) const
{
	Object result;
	switch (type) {
	case __Object::DOMINO_SMALL:
	case __Object::DOMINO_MIDDLE:
	case __Object::DOMINO_LARGE:
		result = __Domino::createDomino(type, matrix, mass, material, freezeState);
		break;
	case __Object::BOX:
		result = __Object::createBox(matrix, size.x, size.y, size.z, mass, material, freezeState);
		break;
	case __Object::SPHERE:
		result = __Object::createSphere(matrix, size.x, size.y, size.z, mass, material, freezeState);
		break;
	case __Object::CYLINDER:
		result = __Object::createCylinder(matrix, size.x, size.y, mass, material, freezeState);
		break;
	case __Object::CAPSULE:
		result = __Object::createCapsule(matrix, size.x, size.y, mass, material, freezeState);
		break;
	case __Object::CONE:
		result = __Object::createCone(matrix, size.x, size.y, mass, material, freezeState);
		break;
	case __Object::CHAMFER_CYLINDER:
		result = __Object::createChamferCylinder(matrix, size.x, size.y, mass, material, freezeState);
		break;
	case __Object::COMPOUND:
	case __Object::CONVEX_ASSEMBLY:
	case __Object::CONVEX_HULL:
		{
			using namespace rapidxml;

			file<char> f(fileName.c_str());

			char* m = f.data();

			// TODO add exception handling
			xml_document<> doc;
			doc.parse<0>(m);

			// this is important so we don't parse the template tag but the object or compound tag
			xml_node<>* nodes = doc.first_node();

			// only the first tag is loaded the rest will be ignored
			xml_node<>* node = nodes->first_node();
			std::string type(node->name());

			if (type == "object" || type == "compound") {
				result = __Object::load(node);
				result->setMatrix(matrix);
				return result;
			}
		}
		break;
	default:
		break;
	}
	return result;
}

void Simulation::createInstance(util::KeyAdapter& keyAdapter,
								util::MouseAdapter& mouseAdapter)
{
	destroyInstance();
	s_instance = new Simulation(keyAdapter, mouseAdapter);
	s_instance->m_newObjectType = __Object::NONE;
	s_instance->m_newObjectMaterial = "yellow";
	s_instance->m_newObjectFilename = "";
	s_instance->m_newObjectMass = -1.0f;
	s_instance->m_newObjectFreezeState = false;
}

void Simulation::destroyInstance()
{
	if (s_instance) delete s_instance;
	s_instance = NULL;
}

Simulation::Simulation(util::KeyAdapter& keyAdapter,
						util::MouseAdapter& mouseAdapter)
	: m_keyAdapter(keyAdapter),
	  m_mouseAdapter(mouseAdapter),
	  m_nextID(0)
{
	m_interactionTypes[util::LEFT] = INT_NONE;
	m_interactionTypes[util::RIGHT] = INT_CREATE_OBJECT;
	m_interactionTypes[util::MIDDLE] = INT_DOMINO_CURVE;
	m_world = NULL;
	m_enabled = true;
	m_gravity = -9.81f * 4.0f;
	m_mouseAdapter.addListener(this);
	m_environment = Object();
	m_lightPos = Vec4f(100.0f, 500.0f, 700.0f, 0.0f);
}

Simulation::~Simulation()
{
	clear();
	m_mouseAdapter.removeListener(this);
}

// TODO I didn't find the todo just paste this code where it belongs
/* template load/save */
/*
void Simulation::saveTemplate(const std::string& fileName, __Object& object)
{
	using namespace rapidxml;
	// create document
	xml_document<> doc;

	// create XML declaration
	xml_node<>* declaration = doc.allocate_node(node_declaration);
	doc.append_node(declaration);
	declaration->append_attribute(doc.allocate_attribute("version", "1.0"));
    declaration->append_attribute(doc.allocate_attribute("encoding", "utf-8"));


	// create root element "template"
	xml_node<>* t = doc.allocate_node(node_element, "template");
	doc.append_node(t);
	
	__Object::save(object, t, &doc);

	std::string s;
	print(std::back_inserter(s), doc, 0);

	// save document
	std::ofstream myfile;
	myfile.open (fileName.c_str());
	myfile << s;
	myfile.close();

	// frees all memory allocated to the nodes
	doc.clear();

}

void Simulation::loadTemplate(const std::string& fileName)
{
	using namespace rapidxml;

	file<char> f(fileName.c_str());

	char* m = f.data();
	
	// TODO add exception handling
	xml_document<> doc;
	doc.parse<0>(m);

	// this is important so we don't parse the template tag but the object or compound tag
	xml_node<>* nodes = doc.first_node();

	// only the first tag is loaded the rest will be ignored
	xml_node<>* node = nodes->first_node();
	std::string type(node->name());

	if (type == "object" || type == "compound") {
			Object object = __Object::load(node);
			add(object);
		}
}*/ /* template load/save END */


void Simulation::save(const std::string& fileName)
{
	using namespace rapidxml;
	// create document
	xml_document<> doc;

	// create XML declaration
	xml_node<>* declaration = doc.allocate_node(node_declaration);
	doc.append_node(declaration);
	declaration->append_attribute(doc.allocate_attribute("version", "1.0"));
    declaration->append_attribute(doc.allocate_attribute("encoding", "utf-8"));


	// create root element "level"
	xml_node<>* level = doc.allocate_node(node_element, "level");
	doc.append_node(level);

	
	ObjectList::iterator itr = m_objects.begin();
	for ( ; itr != m_objects.end(); ++itr) {
		// add node paramenter to save method
		__Object::save(*itr->get(), level, &doc);
	}

	// save attribute "gravity" to m_gravity
	// save attribute "environment" __treecollision.save(m_environment)
	if (m_environment)
		__TreeCollision::save((__TreeCollision&)*m_environment.get(), level, &doc);

	std::string s;
	print(std::back_inserter(s), doc, 0);

	// save document
	std::ofstream myfile;
	myfile.open (fileName.c_str());
	myfile << s;
	myfile.close();

	// frees all memory allocated to the nodes
	doc.clear();

}

void Simulation::load(const std::string& fileName)
{
	init();
	using namespace rapidxml;
	
	file<char> f(fileName.c_str());

	char* m = f.data();

	try {

		xml_document<> doc;
		doc.parse<0>(m);

		// this is important so we don't parse the level tag but the object and compound tags
		xml_node<>* nodes = doc.first_node("level");

		// first_node returns 0 if node is not found
		if (nodes) {
			for (xml_node<>* node = nodes->first_node(); node; node = node->next_sibling()) {
				std::string type(node->name());
				if (type == "object" || type == "compound") {
					Object object = __Object::load(node);
					// load m_id from "id"
					object->setID(atoi(node->first_attribute("id")->value()));
					add(object, object->getID());
				}
			}

			// load "environment" and create tree collision from it
			xml_node<>* node = nodes->first_node("environment");
			if (node) {
				m_environment = __TreeCollision::load(node);
				//((__TreeCollision*)m_environment.get())->createOctree();
			}

			// load gravity

			m_clock.reset();
		} // TODO exception handling in else
	} catch( parse_error& e ) {
		std::cout<<"Parse Exception: \""<<e.what()<<"\" caught in \""<<e.where<char>()<<"\""<<std::endl;
		// TODO tell user in the GUI that XML file he is trying to load is invalid / cannot be parsed
	} catch(...) {
		std::cout<<"Caught unknown exception in Simulation::load"<<std::endl;
		// TODO tell user in the GUI that an unknown error occurred
	}
}

void Simulation::init()
{
	clear();
	m_camera.positionCamera(Vec3f(0.0f, 10.0f, 0.0f), -Vec3f::zAxis(), Vec3f::yAxis());

	m_world = NewtonCreate();
	NewtonWorldSetUserData(m_world, this);

	NewtonSetPlatformArchitecture(m_world, 3);

	// set a fixed world size
	Vec3f minSize(-2000.0f, -2000.0f, -2000.0f);
	Vec3f maxSize(2000.0f, 2000.0f, 2000.0f);
	NewtonSetWorldSize(m_world, &minSize[0], &maxSize[0]);

	NewtonSetSolverModel(m_world, 1);
	NewtonSetFrictionModel(m_world, 1);
	NewtonSetThreadsCount(m_world, 2);
	NewtonSetMultiThreadSolverOnSingleIsland(m_world, 0);

	int id = NewtonMaterialGetDefaultGroupID(m_world);
	NewtonMaterialSetCollisionCallback(m_world, id, id, NULL, NULL, MaterialMgr::GenericContactCallback);

	__Domino::genDominoBuffers(m_vertexBuffer);
	m_skydome.load(2000.0f, "clouds", "skydome", "data/models/skydome.3ds", "flares");

	//m_environment = Object(new __TreeCollision(Mat4f::translate(Vec3f(0.0f, 0.0f, 0.0f)), "data/models/mattest.3ds"));
	//add(m_environment);
	//((__TreeCollision*)m_environment.get())->createOctree();


/*
	Mat4f matrix = Mat4f::translate(Vec3f(5.0f, 5.0f, -5.0f));
	Object obj = __Object::createSphere(matrix, 2.0f, 1.0f, "yellow");
	add(obj);

	matrix =
			Mat4f::rotZ(15.0f * PI / 180.0f) *
			Mat4f::rotX(-90.0f * PI / 180.0f) *
			Mat4f::rotY(25.0f * PI / 180.0f) *
			Mat4f::translate(Vec3f(-5.0f, 5.0f, -5.0f));

	obj = __Object::createBox(matrix, 2.0f, 1.0f, 2.0f, 1.0f, "yellow");
	add(obj);
*/

	// newtons cradle
	if (0)
	{
		const int radius = 2.0f;
		RigidBody boxes[5];
		RigidBody spheres[5];

		Compound c(new __Compound(Mat4f::identity()));//translate(Vec3f(0.0f, 5.0f, -10.0f))));
		RigidBody top = __Object::createBox(Mat4f::translate(Vec3f(0.0f, 4.5f, 0.0f)), radius * 4.2f * 2.0f, 0.5f, 0.5f, 0.0f, "cradle");
		c->add(top);

		for (int x = -2; x <= 2; ++x) {
			boxes[x + 2] = __Object::createBox(Vec3f(x * radius * 2.0f, 2.0f, 0.0f), 0.05f, 5.0f, 0.15f, 0.05f, "cradle");
			spheres[x + 2] = __Object::createSphere(Vec3f(x * radius * 2.0f, 0.0f, 0.0f), radius, 5.0f, "cradle");

			c->add(boxes[x + 2]);
			c->add(spheres[x + 2]);

			Vec3f dir(0.0f, 0.0f, 1.0f);

			c->createHinge(spheres[x + 2]->getMatrix().getW(), dir, boxes[x + 2], spheres[x + 2]);
			Vec3f pivot = boxes[x + 2]->getMatrix().getW() + Vec3f(0.0f, 2.4f, 0.0f);
			c->createHinge(pivot, dir, top, boxes[x + 2]);
		}
		add(c);
		//Mat4f::rotY(90.0f * PI / 180.0f) *
		c->setMatrix(Mat4f::rotY(90.0f * PI / 180.0f) * Mat4f::translate(Vec3f(0.0f, 2.4f, 100.0f)));
		//c->convexCastPlacement();
	}

	// assemlby vs hull comparison
	if(0)
	{
		Object convex = Object(new __ConvexHull(Mat4f::translate(Vec3f(0.0f, 0.0f, -25.0f)), 2.0f, "tire", "data/models/tire.3ds", true));
		add(convex);
		convex->convexCastPlacement();

		convex = Object(new __ConvexAssembly(Mat4f::translate(Vec3f(20.0f, 20.0f, -25.0f)), 2.0f, "tire", "data/models/tire.3ds", __ConvexAssembly::ORIGINAL));
		add(convex);
		convex->convexCastPlacement();
	}

	// simple seesaw with hinge
	if (0)
	{
		Compound c = Compound(new __Compound());
		Object obj0 = __Object::createBox(Mat4f::rotZ(45.0f * PI / 180.0f) * Mat4f::translate(Vec3f(0.0f, -0.5f, 0.0f)), 2.0f, 2.0f, 4.0f, 0.0f, "plankso");
		Object obj1 = __Object::createBox(Vec3f(0.0f, 0.950f, 0.0f), 10.0f, 0.05f, 3.0f, 2.5f, "planks");
		c->add(obj0);
		c->add(obj1);
		c->createHinge(Vec3f(0.0f, 0.950f, 0.0f), Vec3f::zAxis(), obj0, obj1);
		add(c);
		c->setMatrix(c->getMatrix() * Mat4f::translate(Vec3f(10.0f, 0.50f + 0.70f, 50.0f)));
		//c->convexCastPlacement();
	}

	// hinge door
	if (0) {
		const float vertical = 20.0f;
		const float doorHeight = 3.0f;
		Compound c = Compound(new __Compound());

		RigidBody top = __Object::createBox(Mat4f::translate(Vec3f(0.0f, vertical, 0.0f)), 5.0f, 0.5f, 0.5f, 0.0f, "plankso");
		c->add(top);

		RigidBody door = __Object::createBox(Mat4f::translate(Vec3f(0.0f, vertical - doorHeight * 0.5f - 0.25f, 0.0f)), 4.0f, doorHeight, 0.25f, 0.5f, "wood");
		c->add(door);

		c->createHinge(Vec3f(0.0f, vertical - 0.25f, 0.0f), Vec3f::xAxis(), door, top, true, -75.0f * 3.14f / 180.0f, 75.0f * 3.14f / 180.0f);

		add(c);
	}

	// swing
	if (0) {
		const int linksCount = 15;
		const float vertical = 20.0f;
		Vec3f size(1.0f, 0.05f, 0.15f);
		Mat4f llocation = Mat4f::rotZ(90.0f * 3.141592f / 180.0f);
		llocation.setW(Vec3f(-2.0f, vertical - size.x*0.5f, 0.0f));
		Mat4f rlocation = Mat4f::rotZ(90.0f * 3.141592f / 180.0f);
		rlocation.setW(Vec3f(2.0f, vertical - size.x*0.5f, 0.0f));
		RigidBody lobj0, lobj1, robj0, robj1;

		Compound c = Compound(new __Compound());

		RigidBody top = __Object::createBox(Mat4f::translate(Vec3f(0.0f, vertical, 0.0f)), 5.0f, 0.5f, 0.5f, 0.0f, "plankso");
		c->add(top);
		lobj0 = robj0 = top;

		for (int i = 0; i < linksCount; i ++) {
			//create the rigid bodies
			lobj1 = __Object::createCylinder(llocation, size.y, size.x, 0.25f, "rope", 0, Vec4f());
			c->add(lobj1);

			robj1 = __Object::createCylinder(rlocation, size.y, size.x, 0.25f, "rope", 0, Vec4f());
			c->add(robj1);

			// left joint
			Vec3f pivot = llocation.getW();
			pivot.y += (size.x - size.y) * 0.5f;

			c->createBallAndSocket(pivot, llocation.getY(), lobj1, lobj0);//, true, 360.0f * 3.14f / 180.0f, -45.0f * 3.14f / 180.0f, 45.0f * 3.14f / 180.0f);
			lobj0 = lobj1;
			llocation._42 -= (size.x - size.y);

			// right joint
			pivot = rlocation.getW();
			pivot.y += (size.x - size.y) * 0.5f;

			c->createBallAndSocket(pivot, rlocation.getY(), robj1, robj0);//, true, 360.0f * 3.14f / 180.0f, -45.0f * 3.14f / 180.0f, 45.0f * 3.14f / 180.0f);
			robj0 = robj1;
			rlocation._42 -= (size.x - size.y);
		}
		// Mat4f::translate(Vec3f(0.0f, location._42 - (size.x - size.y) * linksCount + size.x*0.5f, 0.0f))
		RigidBody bottom = __Object::createBox(Mat4f::translate(Vec3f(0.0f, llocation._42+size.x*0.5f, 0.0f)), 4.5f, 0.2f, 1.5f, 2.0f, "planks");
		c->add(bottom);

		// left attachment
		Vec3f pivot = llocation.getW();
		pivot.y = llocation._42+size.x*0.5f;
		c->createBallAndSocket(pivot, llocation.getY(), bottom, lobj0, true);

		// right attachment
		pivot = rlocation.getW();
		pivot.y = rlocation._42+size.x*0.5f;
		c->createBallAndSocket(pivot, rlocation.getY(), bottom, robj0, true);

		add(c);
		c->setMatrix(c->getMatrix() * Mat4f::translate(Vec3f(10.0f, 0.0f, 50.0f)));
	}

	// rope way
	if (0) {
		const float anchorRadius = 0.05f;
		const float ropeHeight = 0.25f;
		const float ropeLength = 20.0f;
		const float anchorRopeLength = 5.0f;
		Compound c = Compound(new __Compound());

		// rope
		RigidBody rope = __Object::createBox(Mat4f::identity(), ropeLength, ropeHeight, 0.25f, 0.0f, "metal");
		c->add(rope);

		// anchor
		RigidBody anchor = __Object::createSphere(Mat4f::translate(Vec3f(0.0f, -anchorRadius - ropeHeight*0.5f - 0.1f, 0.0f)), anchorRadius, 1.0f, "wood");
		c->add(anchor);
		c->createSlider(Vec3f(0.0f, -ropeHeight*0.5f, 0.0f), Vec3f(1.0f, 0.0f, 0.0f), anchor, rope, true, -ropeLength*0.5f, ropeLength*0.5f);

		// anchor rope
		RigidBody anchorRope = __Object::createCylinder(
				Mat4f::rotZ(3.14f*0.5f) * Mat4f::translate(Vec3f(0.0f, -anchorRopeLength*0.5f - ropeHeight*0.5f - 0.1f, 0.0f)),
				anchorRadius * 2.0f, anchorRopeLength, 0.1f, "rope");
		c->add(anchorRope);

		c->createBallAndSocket(anchor->getMatrix().getW(), Vec3f::zAxis(), anchorRope, anchor);

		RigidBody load = __Object::createSphere(anchorRope->getMatrix().getW() - Vec3f(0.0f, anchorRopeLength*0.5f, 0.0f), 1.0f, 1.0f, "cradle");
		c->add(load);
		c->createBallAndSocket(load->getMatrix().getW(), Vec3f::zAxis(), load, anchorRope);

		c->setMatrix(c->getMatrix() * Mat4f::translate(Vec3f(0.0f, 10.0f, 0.0f)));
		add(c);
	}

	// rope
	if (0)
	{
		RigidBody obj0, obj1;
		const int linksCount = 15;
		Vec3f size(1.0f, 0.05f, 0.15f);
		Mat4f location = Mat4f::rotZ(90.0f * 3.141592f / 180.0f);
		location._42 = linksCount * (size.x - size.y * 0.5f) + newton::getVerticalPosition(m_world, location._41, location._43) + 2.0f;
		Compound rope = Compound(new __Compound());
		obj0 = __Object::createBox(location * Mat4f::translate(Vec3f(0.0f, size.x * 0.5f, 0.0f)), 0.0f, 0.0f, 0.0f, 0.0f, "", 1);
		rope->add(obj0);

		// create a long vertical rope with limits
		for (int i = 0; i < linksCount; i ++) {
			//create the rigid body
			obj1 = __Object::createCylinder(location, size.y, size.x, 2.0f, "rope");
			rope->add(obj1);
			float dampValue = 0.0f;
			NewtonBodySetLinearDamping(obj1->m_body, dampValue);

			dampValue = 0.1f;
			Vec3f angularDamp(dampValue, dampValue, dampValue);
			NewtonBodySetAngularDamping (obj1->m_body, &angularDamp[0]);

			Vec3f pivot = location.getW();
			pivot.y += (size.x - size.y) * 0.5f;

			rope->createBallAndSocket(pivot, location.getY(), obj1, obj0);
			obj0 = obj1;
			location._42 -= (size.x - size.y);
		}
		Vec3f pivot = location.getW();
		pivot.y += (size.x - size.y) * 0.5f;
		obj1 = __Object::createSphere(location, 0.5f, 5.0f, "wood");
		rope->add(obj1);
		rope->createBallAndSocket(pivot, location.getY(), obj1, obj0, true);
		add(rope);
	}

	// wagon with tires and hinges
	if (0)
	{
		// hinge pinDir is z Axis

		// rotate tires 90 degrees around Y
		Mat4f rot = Mat4f::rotY(90.0f * PI / 180.0f);

		// chassis size
		Vec3f size(5.0f, 0.5f, 2.5f);

		// tire size, z = mass
		Vec3f tires(0.75f, 0.2f, 0.1f);
		Compound c = Compound(new __Compound());

		Object chassis = __Object::createBox(Mat4f::identity(), size.x, size.y, size.z, 5.0f, "yellow");
		c->add(chassis);

		Object fl = __Object::createChamferCylinder(rot * Mat4f::translate(Vec3f(-size.x * 0.5f + 0.25f, 0.0f, -size.z * 0.5f - tires.y * 0.5f - 0.025f)), tires.x, tires.y, tires.z, "yellow");
		c->add(fl);

		c->createHinge(fl->getMatrix().getW(), Vec3f::zAxis(), chassis, fl);
		Object fr = __Object::createChamferCylinder(rot * Mat4f::translate(Vec3f(size.x * 0.5f - 0.25f, 0.0f, -size.z * 0.5f - tires.y * 0.5f - 0.025f)), tires.x, tires.y, tires.z, "yellow");
		c->add(fr);

		c->createHinge(fr->getMatrix().getW(), Vec3f::zAxis(), chassis, fr);
		Object bl = __Object::createChamferCylinder(rot * Mat4f::translate(Vec3f(-size.x * 0.5f + 0.25f, 0.0f, size.z * 0.5f + tires.y * 0.5f + 0.025f)), tires.x, tires.y, tires.z, "yellow");
		c->add(bl);

		c->createHinge(bl->getMatrix().getW(), Vec3f::zAxis(), chassis, bl);
		Object br = __Object::createChamferCylinder(rot * Mat4f::translate(Vec3f(size.x * 0.5f - 0.25f, 0.0f, size.z * 0.5f + tires.y * 0.5f + 0.025f)), tires.x, tires.y, tires.z, "yellow");
		c->add(br);

		c->createHinge(br->getMatrix().getW(), Vec3f::zAxis(), chassis, br);
		add(c);

		c->setMatrix(Mat4f::rotY(PI*0.5f) * Mat4f::translate(Vec3f(0.0f, 0.0f, -25.0f)));
		c->convexCastPlacement();

		/*
		Compound c = Compound(new __Compound());
		Object chassis = __Object::createBox(Mat4f::identity(), 5.0f, 0.5f, 2.5f, 1.0f, "yellow");
		c->add(chassis);
		Object fl = __Object::createCylinder(Mat4f::rotY(90.0f * PI / 180.0f) * Mat4f::translate(Vec3f(-2.25f, -0.0f, -1.25f - 0.125f)), 0.75f, 0.2f, 0.5f, "yellow");
		c->add(fl);
		c->createHinge(fl->getMatrix().getW(), Vec3f::zAxis(), chassis, fl);
		Object fr = __Object::createCylinder(Mat4f::rotY(90.0f * PI / 180.0f) * Mat4f::translate(Vec3f( 2.25f, -0.0f, -1.25f - 0.125f)), 0.75f, 0.2f, 0.5f, "yellow");
		c->add(fr);
		c->createHinge(fr->getMatrix().getW(), Vec3f::zAxis(), chassis, fr);
		Object bl = __Object::createCylinder(Mat4f::rotY(90.0f * PI / 180.0f) * Mat4f::translate(Vec3f(-2.25f, -0.0f, 1.25f + 0.125f)), 0.75f, 0.2f, 0.5f, "yellow");
		c->add(bl);
		c->createHinge(bl->getMatrix().getW(), Vec3f::zAxis(), chassis, bl);
		Object br = __Object::createCylinder(Mat4f::rotY(90.0f * PI / 180.0f) * Mat4f::translate(Vec3f( 2.25f, -0.0f, 1.25f + 0.125f)), 0.75f, 0.2f, 0.5f, "yellow");
		c->add(br);
		c->createHinge(br->getMatrix().getW(), Vec3f::zAxis(), chassis, br);
		add(c);
		c->setMatrix(c->getMatrix() * Mat4f::translate(Vec3f(0.0f, 5.0f, 0.0f)));
		 */
	}

	// bridge
	if (0) {
		const int linksCount = 20;
		Vec3f boxSize(3.0f, 1.0f, 3.0f);
		Vec3f linkSize(1.4f, 0.1f, 0.5f);
		boxSize *= 5.0f;
		linkSize *= 5.0f;

		Mat4f location = Mat4f::identity();

		Compound c = Compound(new __Compound());

		RigidBody link0 = __Object::createBox(location, boxSize.x, boxSize.y, boxSize.z, 0.0f, "wood");
		c->add(link0);
		RigidBody link1;

		location._43 += boxSize.z * 0.5f + linkSize.z * 0.5f;
		location._42 += boxSize.y * 0.5f - linkSize.y * 0.5f;;

		for (int i = 0; i < linksCount; ++i) {

			link1 = __Object::createBox(location, linkSize.x, linkSize.y, linkSize.z, 0.25f, "plankso", 0, Vec4f(0.5f, 0.5f, 0.5f, 0.5f));
			c->add(link1);

			Vec3f pivot(location.getW());
			pivot.y += linkSize.y * 0.5f;
			pivot.z -= linkSize.z * 0.5f;

			c->createHinge(pivot, Vec3f::xAxis(), link0, link1);

			link0 = link1;
			location._43 += linkSize.z;
		}

		Vec3f pivot(location.getW());
		pivot.y += linkSize.y * 0.5f;
		pivot.z -= linkSize.z * 0.5f;

		location._43 += boxSize.z * 0.5f - linkSize.z * 0.5f;
		location._42 += linkSize.y * 0.5f - boxSize.y * 0.5f;

		link1 = __Object::createBox(location, boxSize.x, boxSize.y, boxSize.z, 0.0f, "wood");
		c->add(link1);

		c->createHinge(pivot, Vec3f::xAxis(), link0, link1);

		add(c);
		c->setMatrix(c->getMatrix() * Mat4f::translate(Vec3f(0.0f, 10.0f, 0.0f)));
	}

	//save("data/levels/test_level.xml");
	setEnabled(false);
}

void Simulation::clear()
{
	m_sortedBuffers.clear();
	m_vertexBuffer.flush();
	m_objects.clear();
	m_environment = Object();
	__Domino::freeCollisions();
	m_skydome.clear();
	if (m_world) {
		std::cout << "Remaining bodies: " << NewtonWorldGetBodyCount(m_world) << std::endl;
		NewtonDestroy(m_world);
		std::cout << "Remaining memory: " << NewtonGetMemoryUsed() << std::endl;
	}
	m_world = NULL;
}

int Simulation::add(const Object& object)
{
	return add(object, m_nextID++);
}

int Simulation::add(const Object& object, int id)
{
	if (!object.get())
		return -1;

	object->setID(id);
	ObjectList::iterator begin = m_objects.insert(m_objects.end(), object);

	upload(begin, m_objects.end());
	return id;
}

int Simulation::add(const ObjectInfo& info)
{
	Mat4f matrix(Vec3f::yAxis(), m_camera.viewVector(), m_pointer);
	int result = add(info.create(matrix));
	m_objects.back()->convexCastPlacement();
	return result;
}

void Simulation::remove(const Object& object)
{
	// check if it is a compound, if so we have to check whether
	// one of its children is in a sub-mesh

	// the offset that has to be subtracted from the index offset
	// of all sub-meshes after the sub-mesh that has been deleted.
	// this is necessary, because index data will be removed from
	// the global index list
	unsigned indexOffset = 0;

	// the offset that has to be subtracted from the individual index
	// values of the indices after the deleted sub-mesh. this has to be
	// done because data will be removed from the global vertex list
	unsigned dataOffset = 0;

	// delete vertices only once per object, because there may be
	// multiple sub-meshes with the same data and different materials
	std::set<__Object*> deleted;

	/// @todo think about what happens if a domino is inside a compound
	if (object->getType() <= __Object::DOMINO_LARGE) {
		for (ogl::SubBuffers::iterator it = m_vertexBuffer.m_buffers.begin(); it != m_vertexBuffer.m_buffers.end(); ++it) {
			if ((*it)->object == object.get()) {
				delete (*it);
				m_vertexBuffer.m_buffers.erase(it);
				break;
			}
		}
	} else {
		// iterate over all sub-buffers and check if it has to be deleted
		for (ogl::SubBuffers::iterator it = m_vertexBuffer.m_buffers.begin();
				it != m_vertexBuffer.m_buffers.end(); ) {

			__Object* curObj = (__Object*)(*it)->object;
			if (curObj == NULL) {
				++it;
				continue;
			}

			// Check if a domino is inside a compound (it cannot be a single domino,
			// because we checked that prior to the for-loop
			if (curObj->getType() <= __Object::DOMINO_LARGE) {
				if (object->contains(curObj)) {
					// delete sub-mesh
					delete (*it);
					it = m_vertexBuffer.m_buffers.erase(it);
				}
				// do not adjust anything, because domino data is stored globally
				++it;
			} else {

				// adjust the offsets according to the previously deleted buffers
				(*it)->indexOffset -= indexOffset;
				(*it)->dataOffset -= dataOffset;

				// delete the data associated with the object
				if (object->contains(curObj)) {
					const unsigned co = (*it)->indexOffset;
					const unsigned cc = (*it)->indexCount;
					indexOffset += cc;

					// delete vertices
					if (deleted.find(curObj) == deleted.end()) {
						const unsigned vo = (*it)->dataOffset * m_vertexBuffer.floatSize();
						const unsigned vc = (*it)->dataCount * m_vertexBuffer.floatSize();
						dataOffset += (*it)->dataCount;
						m_vertexBuffer.m_data.erase(m_vertexBuffer.m_data.begin() + vo,
								m_vertexBuffer.m_data.begin() + vo + vc);
						deleted.insert(curObj);
					}

					// delete indices
					m_vertexBuffer.m_indices.erase(m_vertexBuffer.m_indices.begin() + co,
							m_vertexBuffer.m_indices.begin() + co + cc);

					// delete sub-mesh
					delete (*it);
					it = m_vertexBuffer.m_buffers.erase(it);
				} else {
					// adjust the indices of the object, because vertices were deleted
					if (dataOffset > 0) {
						for (unsigned i = 0; i < (*it)->indexCount; ++i)
							m_vertexBuffer.m_indices[i + (*it)->indexOffset] -= dataOffset;
					}
					++it;
				} /* end check if object has to be deleted */
			} /* end check if domino is in compound */
		} /* end for-loop */
	} /* end check if object is domino */

	m_objects.remove(object);

	if (m_environment == object)
		m_environment = Object();
	if (m_selectedObject == object)
		m_selectedObject = Object();

	// upload, because the index offset of the sub-meshes in not correct with
	// regard to the current indices in the element buffer (we deleted some indices)
	upload(m_objects.end(), m_objects.end());
}

static bool isSharedBuffer(const ogl::SubBuffer* const buffer)
{
	return buffer->object == NULL;
}

void Simulation::upload(const ObjectList::iterator& begin, const ObjectList::iterator& end)
{
	for (ObjectList::iterator itr = begin; itr != end; ++itr) {
		(*itr)->genBuffers(m_vertexBuffer);
	}
	m_vertexBuffer.upload();
	m_sortedBuffers.assign(m_vertexBuffer.m_buffers.begin(), m_vertexBuffer.m_buffers.end());
	m_sortedBuffers.remove_if(isSharedBuffer);
	m_sortedBuffers.sort();
}

void Simulation::updateObject(const Object& object)
{
	// We have to keep a reference of the object in order for it
	// to remain in memeory
	bool reselect = m_selectedObject == object;
	Object temp = object;
	remove(object);
	add(temp);
	if (reselect)
		m_selectedObject = temp;
}

Object Simulation::selectObject(int x, int y)
{
	m_camera.apply();

	// Get the word coordinates at the viewport position, i.e.
	// the the 3D position the mouse pointer is
	Vec3f world = m_camera.pointer(x, y);
	Vec3f origin = m_camera.m_position;

	// Cast a ray from the camera position in the direction of the
	// selected world position
	NewtonBody* body = newton::getRayCastBody(m_world, origin, world - origin);

	// find the matching body
	ObjectList::iterator itr = m_objects.begin();
	for ( ; body && itr != m_objects.end(); ++itr) {
		if ((*itr)->contains(body))
			return *itr;
	}

	// nothing was selected, return an empty smart pointer
	Object result;
	return result;
}

void Simulation::mouseMove(int x, int y)
{
	if (m_mouseAdapter.isDown(util::LEFT)) {
		float angleX = (m_mouseAdapter.getX() - x) * 0.075f;
		float angleY = (m_mouseAdapter.getY() - y) * 0.1f;

		m_camera.rotate(angleX, Vec3f::yAxis());
		m_camera.rotate(angleY, m_camera.m_strafe);
	} else if (m_mouseAdapter.isDown(util::RIGHT) && m_enabled) {
		newton::mousePick(m_world, m_camera, Vec2f(x, y), m_mouseAdapter.isDown(util::RIGHT));
	}

	util::Button button = util::LEFT;
	if (m_mouseAdapter.isDown(util::MIDDLE))
		button = util::MIDDLE;
	if (m_mouseAdapter.isDown(util::RIGHT))
		button = util::RIGHT;

	// handle interaction mode if object is selected and in pause mode
	if ((m_interactionTypes[button] == INT_ROTATE ||
			m_interactionTypes[button] == INT_ROTATE_GROUND ||
			m_interactionTypes[button] == INT_MOVE_GROUND ||
			m_interactionTypes[button] == INT_MOVE_BILLBOARD) &&
			m_selectedObject && !m_enabled) {

		// get first position of the mouse (last pos) on the object using pointer
		// get the second pos of the mouse (cur pos) in the world
		// shoot a ray from cam pos to second pos and intersect with the plane
		// move from pos1 to intersection point

		Vec3f pos1 = m_camera.pointer(m_mouseAdapter.getX(), m_mouseAdapter.getY());
		Vec3f pos2 = m_camera.pointer(x, y);

		if (m_interactionTypes[button] == INT_ROTATE) {
			//rot_drag_cur = pos2;
			rot_drag_cur = rot_mat_start.getW() + (pos2-rot_mat_start.getW()).normalized() * (rot_drag_start-rot_mat_start.getW()).len();

		} else if (m_interactionTypes[button] == INT_MOVE_GROUND ||
				m_interactionTypes[button] == INT_MOVE_BILLBOARD ||
				m_interactionTypes[button] == INT_ROTATE_GROUND) {

			// bottom plane
			Vec3f p1 = Vec3f(0.0f, pos1.y, 0.0f);
			Vec3f p2 = pos1 + Vec3f::xAxis();
			Vec3f p3 = pos1 + Vec3f::zAxis();

			// billboard plane
			if (m_interactionTypes[button] == INT_MOVE_BILLBOARD) {
				p1 = pos1;
				p2 = pos1 + Vec3f::yAxis();
				p3 = pos1 + m_camera.m_strafe;
			}

			// get new and old rays
			Vec3f R1, R2;
			Vec3f S1, S2;
			ogl::getScreenRay(Vec2d(x, y), R1, R2, m_camera);
			ogl::getScreenRay(Vec2d(m_mouseAdapter.getX(), m_mouseAdapter.getY()), S1, S2, m_camera);

			pos1 = rayPlaneIntersect(S1, S2, p1, p2, p3);
			Vec3f pos3 = rayPlaneIntersect(R1, R2, p1, p2, p3);

			if (m_interactionTypes[button] == INT_ROTATE_GROUND) {
				Vec3f com = m_selectedObject->getMatrix().getW();
				Vec3f axis = (pos1 - com) % (pos3 - com);
				float factor = axis.y > 0.0f ? -1.0f : 1.0f;
				float angle = (pos1 - com).normalized() * (pos3 - com).normalized();
				angle = acos(angle);
				if (angle == angle) {
					Mat4f rot = Mat4f::rotAxis(Vec3f::yAxis() * factor, angle);
					Mat4f matrix = m_selectedObject->getMatrix();
					matrix = rot * matrix;
					m_selectedObject->setMatrix(matrix);
				}
			} else {
				Mat4f matrix = m_selectedObject->getMatrix();
				matrix.setW(matrix.getW() + (pos3 - pos1));
				m_selectedObject->setMatrix(matrix);
			}

			if (m_interactionTypes[button] == INT_MOVE_GROUND) {
				m_selectedObject->convexCastPlacement();
			}
		} /* end MOVE_GROUND, MOVE_BILLBOARD */
	} /* end selectedObject && !enabled */

	m_pointer = m_camera.pointer(x, y);
}

void Simulation::mouseButton(util::Button button, bool down, int x, int y)
{
	m_pointer = m_camera.pointer(x, y);

	if ((m_interactionTypes[button] == INT_ROTATE || m_interactionTypes[button] == INT_ROTATE_GROUND)
			&& m_selectedObject && !m_enabled) {
		rot_drag_start = m_pointer;
		rot_mat_start = m_selectedObject->getMatrix();
		rot_mouse = Vec2i(x, y);
	}

	if (m_interactionTypes[button] == INT_CREATE_OBJECT && !m_enabled && down) {
		ObjectInfo info(m_newObjectType, m_newObjectMaterial, m_newObjectFilename, m_newObjectMass, m_newObjectFreezeState, m_newObjectSize);
		add(info);
	}

	if (m_interactionTypes[button] == INT_DOMINO_CURVE && down && !m_enabled) {
		curve_spline.knots().push_back(Vec2f(m_pointer.x, m_pointer.z));
	}

	if (button == util::RIGHT && m_enabled) {
		newton::mousePick(m_world, m_camera, Vec2f(x, y), down);
		//newton::applyExplosion(m_world, m_pointer, 30.0f, 20.0f);
	}
}

void Simulation::mouseDoubleClick(util::Button button, int x, int y)
{
	m_pointer = m_camera.pointer(x, y);
	if (button == util::LEFT) {
		m_selectedObject = selectObject(x, y);
		if (m_selectedObject == m_environment)
			m_selectedObject = Object();
	}

	if ((m_interactionTypes[button] == INT_ROTATE || m_interactionTypes[button] == INT_ROTATE_GROUND)
			&& m_selectedObject && !m_enabled)
		rot_mat_start = m_selectedObject->getMatrix();

	if (m_interactionTypes[button] == INT_DOMINO_CURVE && !m_enabled) {
		// Remove knots that are too close to each other, this improves the
		// spline and removes unwanted knots when closing the spline
		bool stop = false;
		while (!stop) {
			stop = true;
			if (curve_spline.knots().size() > 2) {
				Vec2f last = curve_spline.knots()[curve_spline.knots().size()-1];
				Vec2f prev = curve_spline.knots()[curve_spline.knots().size()-2];
				if ((last - prev).len() <= 0.5f) {
					curve_spline.knots().pop_back();
					stop = false;
				}
			}
		}
		curve_spline.update();
		// spline
		if (curve_spline.knots().size() > 2) {
			curve_spline.update();
			for (float t = 0.0f; t < curve_spline.table().back().len; t += 4.5f) {
				Vec3f p = curve_spline.getPos(t);
				Vec3f q = curve_spline.getTangent(t).normalized();
				Mat4f matrix(Vec3f::yAxis(), q, p);
				Domino domino = __Domino::createDomino(__Domino::DOMINO_SMALL, matrix, 5.0, m_newObjectMaterial);
				add(domino);
			}
		// line
		} else if (curve_spline.knots().size() == 2) {
			Vec3f start = curve_spline.knots()[0].xz3(0.0f);
			start.y = newton::getVerticalPosition(m_world, start.x, start.z);
			Vec3f end = curve_spline.knots()[1].xz3(0.0f);
			end.y = newton::getVerticalPosition(m_world, end.x, end.z);
			Vec3f dir = (end - start);
			float len = dir.normalize();
			Mat4f matrix(Vec3f::yAxis(), dir, start);
			for (float d = 0.0f; d <= len; d += 4.5f) {
				matrix.setW(start + dir * d);
				Domino domino = __Domino::createDomino(__Domino::DOMINO_SMALL, matrix, 5.0f, m_newObjectMaterial);
				add(domino);
			}
		}
		curve_spline.knots().clear();
		curve_spline.update();
	}
}

void Simulation::mouseWheel(int delta) {
	float step = delta / 800.0f;

	if (m_selectedObject && !m_enabled) {
		Vec3f scale(
				m_keyAdapter.shift() || m_keyAdapter.alt() ? step * 1.0f : 0.0f,
				(!m_keyAdapter.shift() && !m_keyAdapter.ctrl()) || m_keyAdapter.alt() ? step * 1.0f : 0.0f,
				m_keyAdapter.ctrl() || m_keyAdapter.alt() ? step * 1.0f : 0.0f);

		m_selectedObject->scale(scale, true);
		updateObject(m_selectedObject);
	} else {
		m_camera.move(step);
	}
}

void Simulation::update()
{
	float delta = m_clock.get();
	m_clock.reset();

	static float timeSlice = 0.0f;

	if (m_enabled) {
		timeSlice += delta * 1000.0f;

		while (timeSlice > 12.0f) {
			NewtonUpdate(m_world, (12.0f / 1000.0f) * 20.0f);
			timeSlice = timeSlice - 12.0f;
		}
	}
	m_skydome.update(delta);
	float step = delta * (m_keyAdapter.shift() ? 25.f : 10.0f);

	if (m_keyAdapter.isDown('w')) m_camera.move(step);
	if (m_keyAdapter.isDown('a')) m_camera.strafe(-step);
	if (m_keyAdapter.isDown('s')) m_camera.move(-step);
	if (m_keyAdapter.isDown('d')) m_camera.strafe(step);

	if (m_keyAdapter.isDown(0x7F) && m_selectedObject) {
		remove(m_selectedObject);
		m_selectedObject = Object();
	}
}

void Simulation::render()
{
	m_camera.apply();

	m_vertexBuffer.bind();

/*
	const Mat4f bias(0.5f, 0.0f, 0.0f, 0.0f,
					 0.0f, 0.5f, 0.0f, 0.0f,
					 0.0f, 0.0f, 0.5f, 0.0f,
					 0.5f, 0.5f, 0.5f, 1.0f);
	const Mat4f lightProjection = Mat4f::perspective(45.0f, 1.0f, 10.0f, 1024.0f);
	const Mat4f lightModelview;

	const Mat4f transform = bias * lightProjection * lightModelview * m_camera.m_inverse;
*/

	// set some light properties
	GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat diffuse[4] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat specular[4] = { 0.7, 0.7, 0.7, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, &m_lightPos[0]);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);


	ogl::SubBuffers::const_iterator itr = m_sortedBuffers.begin();
	std::string material = "";
	if (itr != m_sortedBuffers.end()) {
		material = (*itr)->material;
	}
	MaterialMgr& mmgr = MaterialMgr::instance();
	mmgr.applyMaterial(material);
	for ( ; itr != m_sortedBuffers.end(); ++itr) {
		const ogl::SubBuffer* const buf = (*itr);
		const __Object* const obj = (const __Object* const)buf->object;
		if (material != buf->material) {
			material = buf->material;
			mmgr.applyMaterial(material);
		}

		glPushMatrix();
		glMultMatrixf(obj->getMatrix()[0]);
		glDrawElements(GL_TRIANGLES, buf->indexCount, GL_UNSIGNED_INT, (void*)(buf->indexOffset * 4));
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//glDrawElements(GL_TRIANGLES, buf->indexCount, GL_UNSIGNED_INT, (void*)&m_vertexBuffer.m_indices[(buf->indexOffset)]);
		glPopMatrix();
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (m_environment)
		m_environment->render();

	glDisable(GL_LIGHTING);
	m_skydome.render(m_camera, m_lightPos.xyz(), newton::getRayCastBody(m_world, m_camera.m_position, m_lightPos.xyz() - m_camera.m_position));

	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.0, 0.0f);



	if (m_selectedObject) {
		Vec3f min, max;
		ObjectList::iterator itr = m_objects.begin();
		for ( ; itr != m_objects.end(); ++itr) {
			if (*itr == m_selectedObject) {
				(*itr)->render();
				(*itr)->getAABB(min, max);
				if (m_camera.testAABB(min, max) == 1)
					glColor3f(1.0f, 1.0f, 0.0f);
				else
					glColor3f(1.0f, 0.0, 0.0f);
				if (m_camera.checkAABB(min, max)) {
					ogl::drawAABB(min, max);
				}
			}
		}
	}

	glDepthMask(GL_FALSE);
	glColor3f(1.0f, 0.0f, 1.0f);

	if (isActivated(INT_ROTATE) && m_selectedObject && !m_enabled) {
		Vec3f origin = m_selectedObject->getMatrix().getW();
		Vec3f axis = (rot_drag_start - origin) % (rot_drag_cur - origin);

		float angle = (rot_drag_start - origin).normalized() * (rot_drag_cur - origin).normalized();
		angle = acos(angle);// * 180.0f / PI;
		int md = abs(rot_mouse.x - m_mouseAdapter.getX()) + abs(rot_mouse.y -m_mouseAdapter.getY());
		if (md > 2 && angle > 0.01f && angle < PI) {
			Mat4f mat = Mat4f::rotAxis(axis, -angle);
			mat = rot_mat_start * mat;
			mat.setW(rot_mat_start.getW());
			m_selectedObject->setMatrix(mat);
		}
		axis = axis.normalized() * 50.0f + origin;
		glBegin(GL_POINTS);
		glVertex3fv(&rot_drag_start[0]);
		glVertex3fv(&rot_drag_cur[0]);
		glEnd();
		glBegin(GL_LINES);
			glVertex3fv(&(m_selectedObject->getMatrix().getW()[0]));
			glVertex3fv(&axis.x);
		glEnd();
	} else if (isActivated(INT_DOMINO_CURVE) && !m_enabled) {
		curve_spline.update();
		curve_spline.renderKnots();
		curve_spline.renderSpline(0.25f);
		curve_spline.renderPoints(5.0f);
	}

	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glVertex3fv(&m_pointer[0]);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glDepthMask(GL_TRUE);
 }

}
