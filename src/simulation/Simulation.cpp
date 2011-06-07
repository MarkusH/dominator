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
#include <simulation/Domino.hpp>

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


	setEnabled(true);
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


void Simulation::mouseMove(int x, int y)
{
	if (m_mouseAdapter.isDown(util::LEFT)) {
		float angleX = (m_mouseAdapter.getX() - x) * 0.075f;
		float angleY = (m_mouseAdapter.getY() - y) * 0.1f;

		m_camera.rotate(angleX, Vec3f::yAxis());
		m_camera.rotate(angleY, m_camera.m_strafe);
	} else if (m_mouseAdapter.isDown(util::RIGHT)) {
		newton::mousePick(m_world, Vec2f(x, y), m_mouseAdapter.isDown(util::RIGHT));

		/*
			if (!m_objects.size()) {
				Mat4f matrix = Mat4f::translate(Vec3f(5.0f, 5.0f, -5.0f));
				Object obj = __Object::createSphere(matrix, 2.0f, 1.0f, "yellow");
				add(obj);
				std::cout << "sp: " << m_vertexBuffer.m_buffers.size() << std::endl;
			} else if (m_objects.size() == 1) {
				// set matrix
				Mat4f matrix =
						Mat4f::rotZ(15.0f * PI / 180.0f) *
						Mat4f::rotX(-90.0f * PI / 180.0f) *
						Mat4f::rotY(25.0f * PI / 180.0f) *
						Mat4f::translate(Vec3f(-5.0f, 5.0f, -5.0f));

				Object obj = __Object::createBox(matrix, 2.0f, 1.0f, 2.0f, 1.0f, "yellow");
				add(obj);
				std::cout << "bx: " << m_vertexBuffer.m_buffers.size() << std::endl;
			} else if (m_objects.size() == 2) {
				m_environment = __Object::createBox(Mat4f::identity(), 1000.0f, 1.0f, 1000.0f, 0.0f, "yellow");
				add(m_environment);
				std::cout << "env: " << m_vertexBuffer.m_buffers.size() << std::endl;
			} else {


				if (m_selectedObject) {
					// get euler angles

					Vec3f angles = m_selectedObject->getMatrix().eulerAngles();
					angles *= 180.0f / PI;
					std::cout << "angles == " << angles << std::endl;
					// try to rotate anew, using the generated values

					Mat4f matrix =
							Mat4f::rotZ(angles.z * PI / 180.0f) *
							Mat4f::rotX(angles.x * PI / 180.0f) *
							Mat4f::rotY(angles.y * PI / 180.0f) *
							Mat4f::translate(m_selectedObject->getMatrix().getW());
					//std::cout << matrix._11 << " " << matrix._22 << " " << matrix._33 << std::endl;
					m_selectedObject->setMatrix(matrix);

				}

			}
		 */
		/*
		if (m_selectedObject) {
			// TODO move object according to the correct world coordinates of the mouse
			float dx = (x - m_mouseAdapter.getX()) * 0.05f;
			float dy = (m_mouseAdapter.getY() - y) * 0.05f;
			//std::cout << "delta " << dx << std::endl;
			Mat4f matrix = m_selectedObject->getMatrix();
			matrix.setW(matrix.getW() +
					m_camera.m_strafe.normalized() * dx +
					Vec3f::yAxis() * dy);
			m_selectedObject->setMatrix(matrix);
		}
		*/
	}

	m_pointer = m_camera.pointer(x, y);
}

void Simulation::mouseButton(util::Button button, bool down, int x, int y)
{
	m_pointer = m_camera.pointer(x, y);
	if (button == util::MIDDLE && down) {
		setEnabled(true);
		Vec3f view = m_camera.viewVector();
/*
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
				//__Object::createBox(matrix, 1.0f, 1.0f, 6.0f, 1.0f, "yellow") :
				//__Object::createSphere(matrix, 0.1f, 1.0f, "wood_matt");
		//obj->setVelocity(view * 10.0f);
		obj->convexCastPlacement();
		add(obj);
		*/

		Mat4f matrix(Vec3f::yAxis(), view, m_pointer);
		Domino domino = __Domino::createDomino(__Domino::DOMINO_SMALL, matrix, 1.0f, "yellow");
		add(domino);
	} else if (button == util::RIGHT) {
		newton::mousePick(m_world, Vec2f(x, y), down);
	}
}

void Simulation::mouseDoubleClick(util::Button button, int x, int y)
{
	m_pointer = m_camera.pointer(x, y);
	if (button == util::LEFT) {
		m_selectedObject = selectObject(x, y);
		if (m_selectedObject) {
			//std::cout << "objs: " << m_objects.size() << std::endl;
			//std::cout << "bufs: " << m_vertexBuffer.m_buffers.size() << std::endl;
			//remove(m_selectedObject);
			//m_selectedObject = Object();
			//std::cout << "objs: " << m_objects.size() << std::endl;
			//std::cout << "bufs: " << m_vertexBuffer.m_buffers.size() << std::endl;
		}
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
	//newton::MousePick(m_world, Vec2f(m_mouseAdapter.getX(), m_mouseAdapter.getY()), m_mouseAdapter.isDown(util::RIGHT), 0.125f, 1.0f);

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
	}

	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glVertex3fv(&m_pointer[0]);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
 }

}
