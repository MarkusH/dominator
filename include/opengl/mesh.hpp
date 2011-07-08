/**
 * @author Markus Doellinger
 * @date Jun 26, 2011
 * @file opengl/mesh.hpp
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

/**
 * A mesh class that loads model files and extracts the geometry. Each
 * sub-mesh of the model file will have the material of its first face.
 * Sub-meshes with the same material will be combined for faster
 * rendering.
 */
class __Mesh {
private:
	// prevent default initialization
	__Mesh();

protected:
	/** The format of the data. Default is T2F_N3F_V3F */
	GLuint m_format;

	/** The geometry of the mesh in the specified format */
	std::vector<float> m_data;

	/** The indices of the mesh */
	std::vector<uint32_t> m_indices;

	/** The sub-meshes of the mesh */
	ogl::SubBuffers m_buffers;
public:
	virtual ~__Mesh();

	/** @return The number of vertices in the mesh */
	unsigned vertexCount();

	/** @return The number of floats for a single vertex */
	unsigned floatSize();

	/** @return The size of a single vertex in bytes */
	unsigned byteSize();

	/** @return A pointer to the first vertex of the mesh */
	const float* firstVertex();

	/** @return The data of the mesh in the specified format */
	virtual const std::vector<float>& getData();

	/** @return The sub-meshes of the mesh */
	virtual const ogl::SubBuffers& getBuffers();

	/**
	 * Inserts the vertices, indices and sub-meshes into the given VBO.
	 *
	 * @param vbo The VBO to insert the data in.
	 */
	virtual void genBuffers(ogl::VertexBuffer& vbo);

	/**
	 * Returns a mesh created from a 3ds file. The userData of the sub-meshes will
	 * be set to the specified void pointer. Optionally stores the original sub-meshes
	 * of the 3ds file in the last parameter. Deletion of these buffers is the responsibility
	 * of the caller.
	 *
	 * @param fileName       The 3ds file
	 * @param userData       The userData for the sub-meshes
	 * @param originalMeshes A container to insert the original sub-meshes, or NULL
	 * @return               A mesh created from the model file
	 */
	static Mesh load3ds(const std::string& fileName, void* userData, ogl::SubBuffers* originalMeshes = NULL);
};

inline
__Mesh::__Mesh()
	: m_format(GL_T2F_N3F_V3F)
{
}

inline
__Mesh::~__Mesh()
{
}

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
