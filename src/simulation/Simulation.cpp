/*
 * Simulation.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Simulation.hpp>
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
	m_mouseAdapter.addListener(this);
	m_environment = Object();
}

Simulation::~Simulation()
{
	clear();
	m_mouseAdapter.removeListener(this);
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


	//int id = NewtonMaterialGetDefaultGroupID(m_world);
	//NewtonMaterialSetCollisionCallback(m_world, id, id, NULL, NULL, GenericContactProcess);
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
	if (!m_objects.size()) {
		Mat4f matrix = Mat4f::translate(Vec3f(5.0f, 5.0f, -5.0f));
		Object obj = __Object::createSphere(matrix, 2.0f, 1.0f, "yellow");
		add(obj);
	} else if (m_objects.size() == 1) {
		// set matrix
		Mat4f matrix =
				Mat4f::rotZ(15.0f * PI / 180.0f) *
				Mat4f::rotX(-90.0f * PI / 180.0f) *
				Mat4f::rotY(25.0f * PI / 180.0f) *
				Mat4f::translate(Vec3f(-5.0f, 5.0f, -5.0f));

		Object obj = __Object::createBox(matrix, 2.0f, 1.0f, 2.0f, 1.0f, "yellow");
		add(obj);
	} else if (m_objects.size() == 2) {
		m_environment = __Object::createBox(Mat4f::identity(), 1000.0f, 1.0f, 1000.0f, 0.0f, "yellow");
		add(m_environment);
	} else {


		if (m_selectedObject) {
			// get euler angles
/*
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
*/
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
