/*
 * mesh.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: Markus Doellinger
 */

#include <opengl/mesh.hpp>
#include <string.h>
#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <boost/foreach.hpp>

namespace ogl {


void __Mesh::genBuffers(ogl::VertexBuffer& vbo)
{
	// get the offset in floats and vertices
	const unsigned vertexSize = vbo.floatSize();
	const unsigned floatOffset = vbo.m_data.size();
	const unsigned vertexOffset = floatOffset / vertexSize;

	// reserve enough data to prevent re-allocation of memory and then copy it
	vbo.m_data.reserve(floatOffset + m_data.size());
	vbo.m_data.resize(floatOffset + m_data.size());
	std::copy(m_data.begin(), m_data.end(), vbo.m_data.begin() + floatOffset);

	BOOST_FOREACH(const ogl::SubBuffer* old, m_buffers) {
		ogl::SubBuffer* buffer = new ogl::SubBuffer();
		buffer->material = old->material;
		buffer->userData = old->userData;

		buffer->dataCount = old->dataCount;
		buffer->dataOffset = old->dataOffset + vertexOffset;
		buffer->indexCount = old->indexCount;
		buffer->indexOffset = old->indexOffset + vbo.m_indices.size();

		vbo.m_buffers.push_back(buffer);
	}

	// copy the indices to the global list and add the offset
	vbo.m_indices.reserve(vbo.m_indices.size() + m_indices.size());
	for (unsigned i = 0; i < m_indices.size(); ++i)
		vbo.m_indices.push_back(vertexOffset + m_indices[i]);
}

// functor to sort meshes by their material
struct MeshSorter {
	bool operator()(Lib3dsMesh* first, Lib3dsMesh* second) {
		std::string f = first->faces ? first->faceL[0].material : "";
		std::string s = second->faces ? second->faceL[0].material : "";
		return f < s;
	}
};

Mesh __Mesh::load3ds(const std::string& fileName, void* userData, ogl::SubBuffers* originalMeshes)
{
	Lib3dsFile* file = lib3ds_file_load(fileName.c_str());
	if (!file)
		return Mesh();

	Mesh result = Mesh(new __Mesh());

	int numFaces = 0;

	// count the faces and sort the meshes by material in order to combine buffers
	std::list<Lib3dsMesh*> meshes;
	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
		numFaces += mesh->faces;
		meshes.push_back(mesh);
	}
	meshes.sort(MeshSorter());

	int vertexCount = numFaces*3;

	// allocate data
	Lib3dsVector* vertices = new Lib3dsVector[vertexCount];
	Lib3dsVector* normals = new Lib3dsVector[vertexCount];
	Lib3dsTexel* uvs = new Lib3dsTexel[vertexCount];

	unsigned finishedFaces = 0;
	unsigned buffer_vOffset = 0;
	unsigned model_vOffset = 0;

	// for each sub-mesh, load the geometry
	for (std::list<Lib3dsMesh*>::const_iterator itr = meshes.begin(); itr != meshes.end(); ++itr) {
		Lib3dsMesh* mesh = *itr;
		std::string faceMaterial = mesh->faces ? mesh->faceL[0].material : "";

		lib3ds_mesh_calculate_normals(mesh, &normals[finishedFaces*3]);

		// for each face, copy the data
		for (unsigned faceIndex = 0; faceIndex < mesh->faces; faceIndex++) {
			Lib3dsFace* face = &mesh->faceL[faceIndex];

			// for each vertex, copy the data
			for (unsigned int i = 0; i < 3; i++) {
				memcpy(&vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
				if (mesh->texelL)
					memcpy(&uvs[finishedFaces*3 + i], mesh->texelL[face->points[i]], sizeof(Lib3dsTexel));
			}

			finishedFaces++;
		}

		// create original sub-meshes, if requested
		if (originalMeshes) {
			ogl::SubBuffer* buffer = new ogl::SubBuffer();
			buffer->userData = userData;
			buffer->material = faceMaterial;

			buffer->dataCount = finishedFaces*3 - model_vOffset;
			buffer->dataOffset = model_vOffset;

			buffer->indexCount = buffer->dataCount;
			buffer->indexOffset = buffer->dataOffset;

			model_vOffset = finishedFaces*3;
			originalMeshes->push_back(buffer);
		}

		// Only create it if the next mesh has another material, or this is the last mesh.
		// Otherwise combine it with the next mesh.
		std::list<Lib3dsMesh*>::const_iterator next = itr; ++next;
		if (next == meshes.end() || ((*next)->faces ? (*next)->faceL[0].material : "") != faceMaterial) {
			ogl::SubBuffer* buffer = new ogl::SubBuffer();
			buffer->userData = userData;
			buffer->material = faceMaterial;

			buffer->dataCount = finishedFaces*3 - buffer_vOffset;
			buffer->dataOffset = buffer_vOffset;

			buffer->indexCount = buffer->dataCount;
			buffer->indexOffset = buffer->dataOffset;

			buffer_vOffset = finishedFaces*3;
			result->m_buffers.push_back(buffer);
		}
	}
	lib3ds_file_free(file);

	// create buffer data
	result->m_data.reserve(vertexCount * (3+3+2));
	result->m_data.resize(vertexCount * (3+3+2));
	for (int i = 0; i < vertexCount; ++i) {
		memcpy(&result->m_data[i * (3+3+2) + 0], uvs[i], 2 * sizeof(float));
		memcpy(&result->m_data[i * (3+3+2) + 2], normals[i], 3 * sizeof(float));
		memcpy(&result->m_data[i * (3+3+2) + 5], vertices[i], 3 * sizeof(float));
	}

	// store indices
	result->m_indices.reserve(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
		result->m_indices.push_back(i);

	delete normals;
	delete uvs;
	delete vertices;

	return result;
}

}
