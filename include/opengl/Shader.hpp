/*
 * Shader.hpp
 *
 *  Created on: May 21, 2011
 *      Author: markus
 */

#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <GL/glew.h>
#ifdef _WIN32
#include <boost/tr1/memory.hpp>
#else
#include <tr1/memory>
#endif

#include <iostream>
namespace ogl {

class Shader;
typedef std::tr1::shared_ptr<Shader> ShaderPtr;

class Shader {
protected:
	// source code of the vertex and fragment shader
	GLchar *m_vertexSource, *m_fragmentSource;

	// opengl handles
	GLuint m_vertexObject, m_fragmentObject;
	GLuint m_programObject;

	// returns the info (error) log
	void getInfoLog(GLuint object);
public:
	Shader(GLchar* vertexSource, GLchar* fragmentSource);
	virtual ~Shader();

	bool compile();
	inline void bind() { glUseProgram(m_programObject); };
	inline static void unbind() { glUseProgram(0); };

	inline void setUniform4fv(const char* uniform, GLfloat* data) {
		glUniform4fv(glGetUniformLocation(m_programObject, uniform), 1, data);
	};

	inline void setUniform1f(const char* uniform, GLfloat data) {
		glUniform1f(glGetUniformLocation(m_programObject, uniform), data);
	};

	inline void setUniform1i(const char* uniform, GLint data) {
		glUniform1i(glGetUniformLocation(m_programObject, uniform), data);
	};

	static ShaderPtr load(std::string vertexFile, std::string fragmentFile);
};


}

#endif /* SHADER_HPP_ */
