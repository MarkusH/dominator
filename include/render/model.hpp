/*
 * render/model.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef RENDER_MODEL_HPP_
#define RENDER_MODEL_HPP_

#include <iostream>

#include <lib3ds/file.h>
#include <GL/glew.h>


class Model3DS {
public:
	Model3DS(const char *filename);
	virtual void Draw() const;
	virtual void CreateVBO();
protected:
	void GetFaces();
	unsigned int m_TotalFaces;
	Lib3dsFile * m_model;
	GLuint m_VertexVBO, m_NormalVBO;
};

#endif /* RENDER_MODEL_HPP_ */
