/*
 * render/model.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef RENDER_MODEL_HPP_
#define RENDER_MODEL_HPP_

#include <iostream>

#ifndef LIB3DS_FILE
#include <lib3ds/file.h>
#endif
#ifndef GL_GLEW
#include <GL/glew.h>
#endif

class Model3DS {
public:
	Model3DS(std::string filename);
	virtual void Draw() const;
	virtual void CreateVBO();
protected:
	void GetFaces();
	unsigned int m_TotalFaces;
	Lib3dsFile * m_model;
	GLuint m_VertexVBO, m_NormalVBO;
};

#endif /* RENDER_MODEL_HPP_ */
