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

namespace ogl {


__Mesh::__Mesh()
{
	m_format = GL_T2F_N3F_V3F;
}

__Mesh::~__Mesh()
{

}

void __Mesh::genBuffers(ogl::VertexBuffer& vbo)
{
	// get the offset in floats and vertices
	const unsigned vertexSize = vbo.floatSize();
	//const unsigned byteSize = vbo.byteSize();
	const unsigned floatOffset = vbo.m_data.size();
	const unsigned vertexOffset = floatOffset / vertexSize;

	vbo.m_data.reserve(floatOffset + m_data.size());
	vbo.m_data.resize(floatOffset + m_data.size());

	memcpy(&vbo.m_data[floatOffset], &m_data[0], m_data.size() * sizeof(float));

	for (ogl::SubBuffers::iterator itr = m_buffers.begin(); itr != m_buffers.end(); ++itr) {
		ogl::SubBuffer* subBuffer = new ogl::SubBuffer();
		subBuffer->material = (*itr)->material;
		subBuffer->object = (*itr)->object;

		subBuffer->dataCount = (*itr)->dataCount;
		subBuffer->dataOffset = (*itr)->dataOffset + vertexOffset;
		subBuffer->indexCount = (*itr)->indexCount;
		subBuffer->indexOffset = (*itr)->indexOffset + vbo.m_indices.size();

		vbo.m_buffers.push_back(subBuffer);
	}

	// copy the indices to the global list and add the offset
	vbo.m_indices.reserve(vbo.m_indices.size() + m_indices.size());
	for (unsigned i = 0; i < m_indices.size(); ++i)
		vbo.m_indices.push_back(vertexOffset + m_indices[i]);
}

struct MeshEntry {
	Lib3dsMesh* mesh;
	std::string materialID;
	MeshEntry() { }
	MeshEntry(Lib3dsMesh* mesh, const char* material) : mesh(mesh), materialID(material) { }
	static bool compare(const MeshEntry& first, const MeshEntry& second) {
		return first.materialID < second.materialID;
	}
};

Mesh __Mesh::load3ds(const std::string& fileName, void* userData, ogl::SubBuffers* originalMeshes)
{
	Lib3dsFile* file = lib3ds_file_load(fileName.c_str());
	if (!file)
		return Mesh();

	Mesh result = Mesh(new __Mesh());

	int numFaces = 0;

	// count the faces
	for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next)
		numFaces += mesh->faces;

	int vertexCount = numFaces*3;

	// sort meshes by the material, this way we can combine several meshes
	// to a single buffer
	std::list<MeshEntry> sortedMeshes;
	{
		for(Lib3dsMesh* mesh = file->meshes; mesh != NULL; mesh = mesh->next) {
			if (mesh->faces > 0) {
				sortedMeshes.push_back(MeshEntry(mesh, mesh->faceL[0].material));
			}
		}
		sortedMeshes.sort(MeshEntry::compare);
	}

	Lib3dsVector* vertices = new Lib3dsVector[vertexCount];
	Lib3dsVector* normals = new Lib3dsVector[vertexCount];
	Lib3dsTexel* uvs = new Lib3dsTexel[vertexCount];

	unsigned finishedFaces = 0;
	unsigned buffer_vOffset = 0;
	unsigned model_vOffset = 0;

	for (std::list<MeshEntry>::iterator itr = sortedMeshes.begin(); itr != sortedMeshes.end(); ++itr) {
		Lib3dsMesh* mesh = itr->mesh;
		std::string faceMaterial = itr->materialID;

		lib3ds_mesh_calculate_normals(mesh, &normals[finishedFaces*3]);

		for(unsigned cur_face = 0; cur_face < mesh->faces; cur_face++) {
			Lib3dsFace* face = &mesh->faceL[cur_face];

			for(unsigned int i = 0; i < 3; i++) {
				memcpy(&vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
				if (mesh->texelL)
					memcpy(&uvs[finishedFaces*3 + i], mesh->texelL[face->points[i]], sizeof(Lib3dsTexel));
			}

			finishedFaces++;
		}

		// create original sub-meshes, if requested
		if (originalMeshes) {
			ogl::SubBuffer* buffer = new ogl::SubBuffer();
			buffer->object = userData;
			buffer->material = faceMaterial;

			buffer->dataCount = finishedFaces*3 - model_vOffset;
			buffer->dataOffset = model_vOffset;

			buffer->indexCount = buffer->dataCount;
			buffer->indexOffset = buffer->dataOffset;

			model_vOffset = finishedFaces*3;
			originalMeshes->push_back(buffer);
		}

		// Only create it if the next mesh has another material, or this is the last mesh
		// otherwise combine it with the next mesh.
		std::list<MeshEntry>::iterator next = itr;
		++next;
		if (next == sortedMeshes.end() || next->materialID != faceMaterial) {
			ogl::SubBuffer* buffer = new ogl::SubBuffer();
			buffer->object = userData;
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

	result->m_indices.reserve(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
		result->m_indices.push_back(i);

	delete normals;
	delete uvs;
	delete vertices;

	return result;
}

}
