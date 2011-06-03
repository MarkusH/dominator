/*
 * Simulation.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Simulation.hpp>
#include <simulation/Compound.hpp>
#include <simulation/Material.hpp>
#include <opengl/Texture.hpp>
#include <opengl/Shader.hpp>
#include <iostream>
#include <newton/util.hpp>

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

	Object convex = Object(new __ConvexHull(Mat4f::translate(Vec3f(0.0f, 20.0f, -25.0f)), 2.0f, "yellow", "data/models/mesh.3ds"));
	add(convex);

	convex = Object(new __ConvexAssembly(Mat4f::translate(Vec3f(20.0f, 20.0f, -25.0f)), 2.0f, "yellow", "data/models/mesh.3ds"));
	add(convex);

	m_environment = __Object::createBox(Mat4f::identity(), 1000.0f, 1.0f, 1000.0f, 0.0f, "yellow");
	add(m_environment);
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
	Vec3f origin = m_camera.m_position.xyz();

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
		m_camera.rotate(angleY, m_camera.m_strafe.xyz());
	} else if (m_mouseAdapter.isDown(util::RIGHT)) {
		if (m_selectedObject) {
			// TODO move object according to the correct world coordinates of the mouse
			float dx = (x - m_mouseAdapter.getX()) * 0.05f;
			float dy = (m_mouseAdapter.getY() - y) * 0.05f;
			//std::cout << "delta " << dx << std::endl;
			Mat4f matrix = m_selectedObject->getMatrix();
			matrix.setW(matrix.getW() +
					m_camera.m_strafe.xyz().normalized() * dx +
					Vec3f::yAxis() * dy);
			m_selectedObject->setMatrix(matrix);
		}
	}

	m_pointer = m_camera.pointer(x, y);
}

void Simulation::mouseButton(util::Button button, bool down, int x, int y)
{
	m_pointer = m_camera.pointer(x, y);
	if (button == util::MIDDLE) {
		Vec3f view = m_camera.viewVector();

		Mat4f matrix(Vec3f::yAxis(), view, m_camera.m_position.xyz());
		RigidBody obj =// (rand() % 2) ?
				//__Object::createBox(matrix, 1.0f, 1.0f, 6.0f, 1.0f, "yellow") :
				__Object::createSphere(matrix, 0.1f, 1.0f, "wood_matt");
		obj->setVelocity(view * 2.0f);
		add(obj);
	}
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

	static float timeSlice = 0.0f;

	if (m_enabled) {
		timeSlice += delta * 1000.0f;

		while (timeSlice > 12.0f) {
			NewtonUpdate(m_world, (12.0f / 1000.0f) * 20.0f);
			timeSlice = timeSlice - 12.0f;
		}
	}

	float step = delta * (m_keyAdapter.shift() ? 10.f : 5.0f);

	if (m_keyAdapter.isDown('w')) m_camera.move(step);
	if (m_keyAdapter.isDown('a')) m_camera.strafe(-step);
	if (m_keyAdapter.isDown('s')) m_camera.move(-step);
	if (m_keyAdapter.isDown('d')) m_camera.strafe(step);
}

void Simulation::applyMaterial(const std::string& material) {
	const Material* const _mat = MaterialMgr::instance().get(material);

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

		ogl::SubBuffers::iterator itr = m_vertexBuffer.m_buffers.begin();
		for ( ; itr != m_vertexBuffer.m_buffers.end(); ++itr) {
			ogl::SubBuffer* buf = (*itr);
			__Object* obj = (__Object*)buf->object;
			applyMaterial(buf->material);
			glPushMatrix();
			glMultMatrixf(obj->getMatrix()[0]);
			glDrawElements(GL_TRIANGLES, buf->indexCount, GL_UNSIGNED_INT, (void*)(buf->indexOffset * 4));
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

	//if (m_selectedObject) {
		ObjectList::iterator itr = m_objects.begin();
		for ( ; itr != m_objects.end(); ++itr) {
			//if (*itr == m_selectedObject)
			(*itr)->render();
		}
	//}

	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glVertex3fv(&m_pointer[0]);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
 }

}
