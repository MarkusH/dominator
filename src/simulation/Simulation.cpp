/*
 * Simulation.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Simulation.hpp>
#include <opengl/Texture.hpp>
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
	m_mouseAdapter.addListener(this);

	m_world = NewtonCreate();
}

Simulation::~Simulation()
{
	m_mouseAdapter.removeListener(this);
}


void Simulation::upload(ObjectMap::iterator begin, ObjectMap::iterator end)
{
	for (ObjectMap::iterator itr = begin; itr != end; ++itr) {
		itr->first->genBuffers(m_vertexBuffer);
	}
	m_vertexBuffer.upload();
	//TODO: sort materials
}

int Simulation::add(Object object)
{
	ObjectMap::iterator begin = m_objects.insert(std::make_pair(object, ++m_nextID)).first;
	upload(begin, m_objects.end());
	return m_nextID;
}

void Simulation::remove(Object object)
{
	m_objects.erase(object);
	/*
	ObjectMap::iterator itr = m_objects.begin();
	for ( ; itr != m_objects.end(); ++itr) {
		if (itr->first == object) {
			m_objects.erase(itr);
			break;
		}
	}
	*/
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
	ObjectMap::iterator itr = m_objects.begin();
	for ( ; body && itr != m_objects.end(); ++itr) {
		if (itr->first->contains(body))
			return itr->first;
	}

	// nothing was selected, return an empty smart pointer
	Object result;
	return result;
}


void Simulation::mouseMove(int x, int y)
{
	float angleX = (m_mouseAdapter.getX() - x) * 0.075f;
	float angleY = (m_mouseAdapter.getY() - y) * 0.1f;

	m_camera.rotate(angleX, Vec3f::yAxis());
	m_camera.rotate(angleY, m_camera.m_strafe.xyz());

	m_pointer = m_camera.pointer(x, y);
}

void Simulation::mouseButton(util::Button button, bool down, int x, int y)
{
	m_pointer = m_camera.pointer(x, y);
	if (!m_objects.size()) {
		Mat4f matrix = Mat4f::translate(Vec3f(5.0f, 0.0f, -5.0f));
		Object obj = __Object::createSphere(matrix, 2.0f, 1.0f, "yellow");
		add(obj);
	} else if (m_objects.size() == 1) {
		// set matrix
		Mat4f matrix =
				Mat4f::rotZ(15.0f * PI / 180.0f) *
				Mat4f::rotX(-90.0f * PI / 180.0f) *
				Mat4f::rotY(25.0f * PI / 180.0f) *
				Mat4f::translate(Vec3f(-5.0f, 0.0f, -5.0f));

		Object obj = __Object::createBox(matrix, 2.0f, 1.0f, 2.0f, 1.0f, "yellow");
		add(obj);
	} else {
		m_selectedObject = selectObject(x, y);
		if (m_selectedObject) {
			// get euler angles
			Vec3f angles = m_selectedObject->getMatrix().eulerAngles();
			angles *= 180.0f / PI;
			std::cout << angles << std::endl;
			// try to rotate anew, using the generated values
			Mat4f matrix =
					Mat4f::rotZ(angles.z * PI / 180.0f) *
					Mat4f::rotX(angles.x * PI / 180.0f) *
					Mat4f::rotY(angles.y * PI / 180.0f) *
					Mat4f::translate(Vec3f(-5.0f, 0.0f, -5.0f));
			m_selectedObject->setMatrix(matrix);
		}
	}
}


void Simulation::mouseWheel(int delta) {
	float step = delta / 800.0;
	m_camera.move(step);
}

void Simulation::update()
{
	float delta = m_clock.get();
	m_clock.reset();

	float step = delta * (m_keyAdapter.shift() ? 2.5f : 1.25f);

	if (m_keyAdapter.isDown('w')) m_camera.move(step);
	if (m_keyAdapter.isDown('a')) m_camera.strafe(-step);
	if (m_keyAdapter.isDown('s')) m_camera.move(-step);
	if (m_keyAdapter.isDown('d')) m_camera.strafe(step);
}

void Simulation::render()
{
	m_camera.update();
	m_camera.apply();

	ogl::TextureMgr::instance().get("yellow")->bind();



	if (m_vertexBuffer.m_vbo && m_vertexBuffer.m_ibo) {
		m_vertexBuffer.bind();

		ogl::SubBuffers::iterator itr = m_vertexBuffer.m_buffers.begin();
		for ( ; itr != m_vertexBuffer.m_buffers.end(); ++itr) {
			ogl::SubBuffer* buf = (*itr);
			__Object* obj = (__Object*)buf->object;
			glPushMatrix();
			glMultMatrixf(obj->getMatrix()[0]);
			glDrawElements(GL_TRIANGLES, buf->indexCount, GL_UNSIGNED_INT, (void*)(buf->indexOffset * 4));
			glPopMatrix();
		}
	}

	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.0, 0.0f);

	ObjectMap::iterator itr = m_objects.find(m_selectedObject);
	if (itr != m_objects.end()) {
		itr->first->render();
	}

	/*
	if (m_selectedObject) {
		ObjectMap::iterator itr = m_objects.begin();
		for ( ; itr != m_objects.end(); ++itr) {
			if (itr->first.get() == select)
			itr->first->render();
		}
	}
	*/

	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glVertex3fv(&m_pointer[0]);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
 }

}
