/*
 * mesh.hpp
 *
 *  Created on: Jun 26, 2011
 *      Author: markus
 */

#ifndef MESH_HPP_
#define MESH_HPP_

#include <m3d/m3d.hpp>
#include <vector>
#include <string>
#include <opengl/vertexbuffer.hpp>
#include <boost/tr1/memory.hpp>

namespace ogl {

using namespace m3d;

class __Mesh;
typedef std::tr1::shared_ptr<__Mesh> Mesh;

class __Mesh {
private:
	__Mesh();

protected:
	std::vector<float> m_data;
	std::vector<uint32_t> m_indices;
	ogl::SubBuffers m_buffers;
	GLuint m_format;
public:
	virtual ~__Mesh();

	/** @return The number of vertices in the mesh */
	unsigned vertexCount();

	/** @return The size of a single vertex in floats */
	unsigned floatSize();

	/** @return The size of a single vertex in bytes */
	unsigned byteSize();

	/** @return A pointer to the first vertex of the mesh */
	const float* firstVertex();

	virtual const std::vector<float>& getData();
	virtual const ogl::SubBuffers& getBuffers();
	virtual void genBuffers(ogl::VertexBuffer& vbo);

	static Mesh load3ds(const std::string& fileName, void* userData, ogl::SubBuffers* originalMeshes = NULL);
};

inline
const std::vector<float>& __Mesh::getData()
{
	return m_data;
}

inline
const ogl::SubBuffers& __Mesh::getBuffers()
{
	return m_buffers;
}

inline
unsigned __Mesh::vertexCount()
{
	return m_data.size() / floatSize();
}

inline
const float* __Mesh::firstVertex()
{
	switch (m_format) {
	case GL_T2F_N3F_V3F:
		return &m_data[2 + 3];
	}
	return NULL;
}

inline
unsigned __Mesh::byteSize()
{
	return floatSize() * 4;
}

inline
unsigned __Mesh::floatSize()
{
	switch (m_format) {
	case GL_T2F_N3F_V3F:
		return (2 + 3 + 3);
	}
	return 0;
}

}

#endif /* MESH_HPP_ */
