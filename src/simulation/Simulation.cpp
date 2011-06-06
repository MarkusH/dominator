/*
 * Simulation.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Simulation.hpp>
#include <simulation/Compound.hpp>
#include <simulation/TreeCollision.hpp>
#include <simulation/Material.hpp>
#include <opengl/Texture.hpp>
#include <opengl/Shader.hpp>
#include <iostream>
#include <newton/util.hpp>

// Mouse pick during running simulation
#define MOUSE_PICK

// Move object during pause
//#define PLANE_MOVE
	// Move object along billboard plane, if not set move along ground
	//#define BILLBOARD_PLANE

// Rotate object using mouse
// CAUTION: do not leave the surface you first clicked on
//#define MOUSE_ROTATE

namespace sim {

Simulation* Simulation::s_instance = NULL;

void Simulation::createInstance(util::KeyAdapter& keyAdapter,
								util::MouseAdapter& mouseAdapter)
{
	destroyInstance();
	s_instance = new Simulation(keyAdapter, mouseAdapter);
};

void Simulation::destroyInstance()
{
	if (s_instance) delete s_instance;
	s_instance = NULL;
};

Simulation& Simulation::instance()
{
	return *s_instance;
};

Simulation::Simulation(util::KeyAdapter& keyAdapter,
						util::MouseAdapter& mouseAdapter)
	: m_keyAdapter(keyAdapter),
	  m_mouseAdapter(mouseAdapter),
	  m_nextID(0)
{
	m_world = NULL;
	m_enabled = true;
	m_gravity = -9.81f * 4.0f;
	m_mouseAdapter.addListener(this);
	m_environment = Object();
}

Simulation::~Simulation()
{
	clear();
	m_mouseAdapter.removeListener(this);
}

void Simulation::save(const std::string& fileName)
{
	// create document and root node "level" or "simulation", depends on you

	ObjectList::iterator itr = m_objects.begin();
	for ( ; itr != m_objects.end(); ++itr) {
		// add node paramenter to save method
		__Object::save(*itr->get());
	}

	// save attribute "gravity" to m_gravity
	// save attribute "environment" to
	// 		m_environment ? m_environment->getID() : -1
}

void Simulation::load(const std::string&fileName)
{
	// parse document

	// for each node object
	// add node paramenter to save method
	Object object = __Object::load();
	add(object, object->getID());

	// load gravity
	// load "environment"
	// m_environment = (id == -1) ? Object() : retrieve object with id

	m_clock.reset();
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

	NewtonSetMultiThreadSolverOnSingleIsland(m_world, 1);


	int id = NewtonMaterialGetDefaultGroupID(m_world);
	NewtonMaterialSetCollisionCallback(m_world, id, id, NULL, NULL, MaterialMgr::GenericContactCallback);


	//m_environment = __Object::createBox(Mat4f::identity(), 1000.0f, 1.0f, 1000.0f, 0.0f, "yellow");
	m_environment = Object(new __TreeCollision(Mat4f::translate(Vec3f(0.0f, 0.0f, 0.0f)), "data/models/ramps.3ds"));
	add(m_environment);

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
		RigidBody boxes[5];
		RigidBody spheres[5];

		Compound c(new __Compound(Mat4f::translate(Vec3f(0.0f, 15.0f, 10.0f))));
		RigidBody top = __Object::createBox(Mat4f::translate(Vec3f(0.0f, 4.5f, 0.0f)), 20.0f, 0.5f, 0.5f, 0.0f, "black");
		c->add(top);

		for (int x = -2; x <= 2; ++x) {
			boxes[x + 2] = __Object::createBox(Vec3f(x * 4.0f, 2.0f, 0.0f), 0.05f, 5.0f, 0.15f, 0.05f, "blue");
			spheres[x + 2] = __Object::createSphere(Vec3f(x * 4.0f, 0.0f, 0.0f), 2.0f, 2.0f, "red");

			c->add(boxes[x + 2]);
			c->add(spheres[x + 2]);

			Vec3f dir(0.0f, 0.0f, 1.0f);

			c->createHinge(spheres[x + 2]->getMatrix().getW(), dir, boxes[x + 2], spheres[x + 2]);
			Vec3f pivot = boxes[x + 2]->getMatrix().getW() + Vec3f(0.0f, 2.4f, 0.0f);
			c->createHinge(pivot, dir, top, boxes[x + 2]);
		}
		add(c);
		c->setMatrix(c->getMatrix() * Mat4f::rotY(45.0f * PI / 180.0f));
		c->convexCastPlacement();
	}

	// assemlby vs hull comparison
	{
		Object convex = Object(new __ConvexHull(Mat4f::translate(Vec3f(0.0f, 0.0f, -25.0f)), 2.0f, "yellow", "data/models/mesh.3ds", false));
		add(convex);
		convex->convexCastPlacement();

		convex = Object(new __ConvexAssembly(Mat4f::translate(Vec3f(20.0f, 20.0f, -25.0f)), 2.0f, "yellow", "data/models/mesh.3ds", __ConvexAssembly::ORIGINAL));
		add(convex);
		convex->convexCastPlacement();
	}

	// simple seesaw with hinge
	if (0)
	{
		Compound c = Compound(new __Compound());
		Object obj0 = __Object::createBox(Mat4f::rotZ(45.0f * PI / 180.0f) * Mat4f::translate(Vec3f(0.0f, -0.5f, 0.0f)), 2.0f, 2.0f, 4.0f, 0.0f, "yellow");
		Object obj1 = __Object::createBox(Vec3f(0.0f, 0.950f, 0.0f), 10.0f, 0.05f, 3.0f, 2.5f, "yellow");
		c->add(obj0);
		c->add(obj1);
		c->createHinge(Vec3f(0.0f, 0.950f, 0.0f), Vec3f::zAxis(), obj0, obj1);
		add(c);
		c->setMatrix(c->getMatrix() * Mat4f::translate(Vec3f(10.0f, 0.75f, 10.0f)));
		c->convexCastPlacement();
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
		// create a long vertical rope with limits
		for (int i = 0; i < linksCount; i ++) {
			// create the a graphic character (use a visualObject as our body

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
		obj1 = __Object::createSphere(location, 0.5f, 5.0f, "rope");
		rope->add(obj1);
		rope->createBallAndSocket(pivot, location.getY(), obj1, obj0);
		add(rope);

	}

	// wagon with tires and hinges
	if (1)
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

#ifdef MOUSE_PICK
	setEnabled(true);
#else
	setEnabled(false);
#endif
}

void Simulation::clear()
{
	m_vertexBuffer.flush();
	m_objects.clear();
	m_environment = Object();
	if (m_world) {
		std::cout << "Remaining bodies: " << NewtonWorldGetBodyCount(m_world) << std::endl;
		NewtonDestroy(m_world);
		std::cout << "Remaining memory: " << NewtonGetMemoryUsed() << std::endl;
	}
}

int Simulation::add(const Object& object)
{
	return add(object, m_nextID++);
}

int Simulation::add(const Object& object, int id)
{
	object->setID(m_nextID++);
	ObjectList::iterator begin = m_objects.insert(m_objects.end(), object);

	upload(begin, m_objects.end());
	return m_nextID-1;
}

void Simulation::remove(Object object)
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

	// iterate over all sub-buffers and check if it has to be deleted
	for (ogl::SubBuffers::iterator it = m_vertexBuffer.m_buffers.begin();
			it != m_vertexBuffer.m_buffers.end(); ) {

		__Object* curObj = (__Object*)(*it)->object;

		// adjust the offsets according to the previously deleted buffers
		(*it)->indexOffset -= indexOffset;
		(*it)->dataOffset -= dataOffset;

		// delete the data associated with the object
		if (curObj->contains(object)) {
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
		}
	}

	m_objects.remove(object);

	if (m_environment == object)
		m_environment = Object();
	if (m_selectedObject == object)
		m_selectedObject = Object();

	// upload, because the index offset of the sub-meshes in not correct with
	// regard to the current indices in the element buffer (we deleted some indices)
	upload(m_objects.end(), m_objects.end());
}

void Simulation::upload(const ObjectList::iterator& begin, const ObjectList::iterator& end)
{
	//std::cout << begin->first << " " << end->first << std::endl;
	for (ObjectList::iterator itr = begin; itr != end; ++itr) {
	//	std::cout << "id " << itr->first << std::endl;
		(*itr)->genBuffers(m_vertexBuffer);
	}
	m_vertexBuffer.upload();
	//TODO: sort materials
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

#ifdef MOUSE_ROTATE
Mat4f start_mat;
Vec3f start_drag;
Vec3f cur_drag;
float mx, my;
#endif

void Simulation::mouseMove(int x, int y)
{
	if (m_mouseAdapter.isDown(util::LEFT)) {
		float angleX = (m_mouseAdapter.getX() - x) * 0.075f;
		float angleY = (m_mouseAdapter.getY() - y) * 0.1f;

		m_camera.rotate(angleX, Vec3f::yAxis());
		m_camera.rotate(angleY, m_camera.m_strafe);
	} else if (m_mouseAdapter.isDown(util::RIGHT)) {
#ifdef MOUSE_PICK
		newton::mousePick(m_world, Vec2f(x, y), m_mouseAdapter.isDown(util::RIGHT));
#endif

		if (m_selectedObject) {

			// get first position of the mouse (last pos) on the object using pointer
			// get the second pos of the mouse (cur pos) in the world
			// shoot a ray from cam pos to second pos and intersect with the plane
			// move from pos1 to intersection point
			//

			Vec3f pos1 = m_camera.pointer(m_mouseAdapter.getX(), m_mouseAdapter.getY());
			Vec3f pos2 = m_camera.pointer(x, y);
#ifdef MOUSE_ROTATE
			cur_drag = pos2;
#endif
#ifdef PLANE_MOVE
			Vec3f R1, R2, S1, S2;
			{
				Vec4<GLint> viewport = Vec4<GLint>::viewport();
				GLfloat z;
				float _y = viewport.w - y;
				Mat4d mvmatrix = Mat4d::modelview();
				Mat4d projmatrix = Mat4d::projection();

				// get new ray
				double dX, dY, dZ;
				gluUnProject ((double) x, _y, 0.0, mvmatrix[0], projmatrix[0], &viewport[0], &dX, &dY, &dZ);
				R1 = Vec3f ( (float) dX, (float) dY, (float) dZ );
				gluUnProject ((double) x, _y, 1.0, mvmatrix[0], projmatrix[0], &viewport[0], &dX, &dY, &dZ);
				R2 = Vec3f ( (float) dX, (float) dY, (float) dZ );

				// get old ray
				_y = viewport.w - m_mouseAdapter.getY();
				gluUnProject ((double) m_mouseAdapter.getX(), _y, 0.0, mvmatrix[0], projmatrix[0], &viewport[0], &dX, &dY, &dZ);
				S1 = Vec3f ( (float) dX, (float) dY, (float) dZ );
				gluUnProject ((double) m_mouseAdapter.getX(), _y, 1.0, mvmatrix[0], projmatrix[0], &viewport[0], &dX, &dY, &dZ);
				S2 = Vec3f ( (float) dX, (float) dY, (float) dZ );
			}

			// billboard matrix
			Vec3f look = m_camera.m_position - pos1;
			Vec3f right = Vec3f::yAxis() % look;
			Vec3f up = Vec3f::yAxis();

			// billboard plane
#ifdef BILLBOARD_PLANE
			Vec3f p1 = pos1;
			Vec3f p2 = pos1 + Vec3f::yAxis();
			Vec3f p3 = pos1 + m_camera.m_strafe;
#else
			// bottom plane
			Vec3f p1 = Vec3f(0.0f, pos1.y, 0.0f);
			Vec3f p2 = pos1 + Vec3f::xAxis();
			Vec3f p3 = pos1 + Vec3f::zAxis();
#endif
			pos1 = rayPlaneIntersect(S1, S2, p1, p2, p3);
			Vec3f pos3 = rayPlaneIntersect(R1, R2, p1, p2, p3);

			Mat4f matrix = m_selectedObject->getMatrix();
			matrix.setW(matrix.getW() + (pos3 - pos1));
			m_selectedObject->setMatrix(matrix);
#endif
		}

	}

	m_pointer = m_camera.pointer(x, y);
}

void Simulation::mouseButton(util::Button button, bool down, int x, int y)
{
	//std::cout << "down" << std::endl;
	m_pointer = m_camera.pointer(x, y);

#ifdef MOUSE_ROTATE
	if (button == util::RIGHT && m_selectedObject) {
		start_drag = m_pointer;
		start_mat = m_selectedObject->getMatrix();
		mx = x; my = y;
	}
#endif

	if (button == util::MIDDLE && down) {
		//setEnabled(true);
		Vec3f view = m_camera.viewVector();

		Mat4f matrix(Vec3f::yAxis(), view, m_camera.m_position);
		RigidBody obj;
		static int counter = 0;
		switch (counter++) {
		case 0:
			obj = __Object::createBox(matrix, 1.0f, 1.0f, 1.0f, 5.0f, "yellow");
			break;
		case 1:
			obj = __Object::createSphere(matrix, 0.2f, 50.0f, "wood_matt");
			break;
		case 2:
			obj = __Object::createCapsule(matrix, 0.5f, 5.0f, 1.0f, "yellow");
			break;
		case 3:
			obj = __Object::createCone(matrix, 0.5f, 1.0f, 1.0f, "yellow");
			break;
		case 4:
			obj = __Object::createCylinder(matrix, 0.5f, 1.0f, 1.0f, "yellow");
			break;
		case 5:
			std::cout << "chamfer "<< std::endl;
			obj = __Object::createChamferCylinder(matrix, 2.0f, 0.5f, 1.0f, "yellow");
			break;
		}
		if (counter == 6)
			counter = 0;

		//obj->setVelocity(view * 10.0f);
		obj->convexCastPlacement();
		add(obj);

	} else if (button == util::RIGHT) {
#ifdef MOUSE_PICK
		newton::mousePick(m_world, Vec2f(x, y), down);
#endif
	}
}

void Simulation::mouseDoubleClick(util::Button button, int x, int y)
{
	m_pointer = m_camera.pointer(x, y);
	if (button == util::LEFT) {
		m_selectedObject = selectObject(x, y);
		if (m_selectedObject && m_selectedObject == m_environment)
			m_selectedObject = Object();
	}
}

void Simulation::mouseWheel(int delta) {
	float step = delta / 800.0f;

	if (m_selectedObject) {
		Mat4f matrix = m_selectedObject->getMatrix();
		matrix.setW(matrix.getW() + m_camera.viewVector() * step);
		m_selectedObject->setMatrix(matrix);
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

	float step = delta * (m_keyAdapter.shift() ? 25.f : 10.0f);

	if (m_keyAdapter.isDown('w')) m_camera.move(step);
	if (m_keyAdapter.isDown('a')) m_camera.strafe(-step);
	if (m_keyAdapter.isDown('s')) m_camera.move(-step);
	if (m_keyAdapter.isDown('d')) m_camera.strafe(step);
}

void Simulation::applyMaterial(const std::string& material) {
	const Material* const _mat = MaterialMgr::instance().get(material);
	//TODO only switch shader/texture if necessary
	if (_mat != NULL) {
		const Material& mat = *_mat;
		ogl::Texture texture = ogl::TextureMgr::instance().get(mat.texture);

		if (texture) {
			glEnable(GL_TEXTURE_2D);
			texture->bind();
		} else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glMaterialfv(GL_FRONT, GL_DIFFUSE, &mat.diffuse[0]);
		glMaterialfv(GL_FRONT, GL_AMBIENT, &mat.ambient[0]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, &mat.specular[0]);
		glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);

		ogl::Shader shader = ogl::ShaderMgr::instance().get(mat.shader);
		if (shader) {
			shader->bind();
		} else {
			ogl::__Shader::unbind();
		}

	} else {
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);
		glUseProgram(0);
	}
}

void Simulation::render()
{
	m_camera.update();
	m_camera.apply();

	if (m_vertexBuffer.m_vbo && m_vertexBuffer.m_ibo) {
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

		ogl::SubBuffers::iterator itr = m_vertexBuffer.m_buffers.begin();
		for ( ; itr != m_vertexBuffer.m_buffers.end(); ++itr) {
			ogl::SubBuffer* buf = (*itr);
			__Object* obj = (__Object*)buf->object;
			applyMaterial(buf->material);
			glPushMatrix();
			glMultMatrixf(obj->getMatrix()[0]);
			glDrawElements(GL_TRIANGLES, buf->indexCount, GL_UNSIGNED_INT, (void*)(buf->indexOffset * 4));
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			//glDrawElements(GL_TRIANGLES, buf->indexCount, GL_UNSIGNED_INT, (void*)&m_vertexBuffer.m_indices[(buf->indexOffset)]);
			glPopMatrix();
		}
	}

	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.0, 0.0f);

	/*
	ObjectMap::iterator itr = m_objects.find(m_selectedObject);
	if (itr != m_objects.end()) {
		itr->second->render();
	}
*/

	if (m_selectedObject) {
		ObjectList::iterator itr = m_objects.begin();
		for ( ; itr != m_objects.end(); ++itr) {
			if (*itr == m_selectedObject)
			(*itr)->render();
		}
#ifdef PLANE_MOVE
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 1.0, 1.0f, 0.5f);
		glDisable(GL_CULL_FACE);
		//if (m_mouseAdapter.isDown(util::RIGHT)) {

			Vec3f pos = m_camera.pointer(m_mouseAdapter.getX(), m_mouseAdapter.getY());
			Vec3f look = m_camera.m_position - pos;
			Vec3f right = m_camera.m_up % look;
			Vec3f up = look % right;

			Vec3f position = pos * Mat4f::modelview();// * Mat4f(right, up, look, Vec3f());
			Mat4f matrix = Mat4f(right, up, look, Vec3f());

			float scale = 50.0f;
			glPushMatrix();
			glLoadIdentity();
			//glMultMatrixf(matrix[0]);
			//glBegin(GL_QUADS);
			//glTexCoord2f(0.0f, 0.0f); glVertex3f(position[0] - scale, position[1] - scale, position[2]);
			//glTexCoord2f(0.0f, 1.0f); glVertex3f(position[0] - scale, position[1] + scale, position[2]);
			//glTexCoord2f(1.0f, 1.0f); glVertex3f(position[0] + scale, position[1] + scale, position[2]);
			//glTexCoord2f(1.0f, 0.0f); glVertex3f(position[0] + scale, position[1] - scale, position[2]);
			//glEnd();
			glPopMatrix();
		//}

			glDisable(GL_BLEND);
#endif
	}

#ifdef MOUSE_ROTATE
	glDepthMask(GL_FALSE);
	glColor3f(1.0f, 0.0f, 1.0f);
	if (m_selectedObject) {
		Vec3f origin = m_selectedObject->getMatrix().getW();
		Vec3f axis = (start_drag - origin) % (cur_drag - origin);

		float angle = (start_drag - origin).normalized() * (cur_drag - origin).normalized();
		angle = acos(angle);// * 180.0f / PI;
		int md = abs(mx -m_mouseAdapter.getX()) + abs(my -m_mouseAdapter.getY());
		if (md >2&&angle > 0.01f && angle < PI) {
			//std::cout << "angle = " << (start_drag - cur_drag).len() << std::endl;
			Mat4f mat = Mat4f::rotAxis(axis, -angle);
			Vec3f euler = mat.eulerAngles();
			//if (euler.x < 2.0f)
			//	mat = Mat4f::rotAxis(Vec3f::xAxis(), -euler.x) * mat;
			//if (euler.y < 2.0f)
			//	mat = Mat4f::rotAxis(Vec3f::yAxis(), -euler.y) * mat;
			//if (euler.z < 2.0f)
			//	mat = Mat4f::rotAxis(Vec3f::zAxis(), -euler.z) * mat;
			mat = start_mat * mat;
			mat.setW(start_mat.getW());
			m_selectedObject->setMatrix(mat);
		}
		axis = axis.normalized() * 50.0f + origin;
		glBegin(GL_POINTS);
		glVertex3fv(&start_drag[0]);
		glVertex3fv(&cur_drag[0]);
		glEnd();
		glBegin(GL_LINES);
			glVertex3fv(&(m_selectedObject->getMatrix().getW()[0]));
			glVertex3fv(&axis.x);
		glEnd();
	}
#endif

	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glVertex3fv(&m_pointer[0]);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glDepthMask(GL_TRUE);
 }

}
