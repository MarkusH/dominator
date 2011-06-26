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
#ifdef _WIN32
#include <boost/tr1/memory.hpp>
#else
#include <tr1/memory>
#endif

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
public:
	Vec3f m_scale;

	virtual ~__Mesh();

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	static Mesh load3ds(const std::string& fileName, void* object, const Vec3f& scale);
};

}

#endif /* MESH_HPP_ */
