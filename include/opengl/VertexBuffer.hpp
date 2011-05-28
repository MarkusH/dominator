/*
 * VertexBuffer.hpp
 *
 *  Created on: May 28, 2011
 *      Author: Markus Doellinger
 */

#ifndef VERTEXBUFFER_HPP_
#define VERTEXBUFFER_HPP_

#include <vector>
#include <list>
#include <string>
#include <GL/glew.h>
#ifdef _WIN32
#include <pstdint.h>
#else
#include <stdint.h>
#endif

namespace ogl {

typedef std::vector<float> Floats;
typedef std::vector<uint32_t> UInts;

/**
 * A logical sub-buffer within a vertex buffer. It is used
 * to control different positions and materials, i.e. different
 * objects within a single VBO.
 */
struct SubBuffer {
	// material of the sub mesh
	std::string material;

	// the object that generated this sub mesh
	void* object;

	// the offset and size in the global index buffer
	uint32_t indexOffset;
	uint32_t indexCount;

	// the offset and size in the global vertex buffer
	uint32_t dataOffset;
	uint32_t dataCount;

	SubBuffer() {
		material = "";
		indexOffset = indexCount = 0;
		dataOffset = dataCount = 0;
		object = NULL;
	}
};

typedef std::list<SubBuffer*> SubBuffers;

/**
 * A vertex buffer with vertex and index data
 */
class VertexBuffer {
public:
	// index and vertex buffers
	GLuint m_ibo, m_vbo;
	// actual size and used size of the buffers
	uint32_t m_vboSize, m_vboUsedSize;
	uint32_t m_iboSize, m_iboUsedSize;

	// the index data
	UInts m_indices;

	// the vertices, uvs and normals
	Floats m_data;

	SubBuffers m_buffers;

	VertexBuffer();
	~VertexBuffer();

	/**
	 * Uploads and creates the buffers.
	 */
	void upload();

	/**
	 * Clears the internal data and destroys the buffers.
	 */
	void flush();
};

}

#endif /* VERTEXBUFFER_HPP_ */
