/*
 * Shader.hpp
 *
 *  Created on: May 21, 2011
 *      Author: Markus Doellinger, Robert Waury
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
#include <map>

namespace ogl {

// forward declaration
class __Shader;

// smart pointer for the shader class
typedef std::tr1::shared_ptr<__Shader> Shader;

/**
 * A class that wraps an OpenGL shader program, i.e. a combination
 * of vertex and fragment shader. It is recommended to always use
 * the smart pointer ShaderPtr instead of a pointer to a shader object.
 *
 * A shader can be constructed using the source code of vertex and
 * fragment shader, or by using the static load() method. After the
 * creation, the shader has to be compiled and linked with the
 * compile() method.
 *
 * To use the shader, call bind() and then (optionally) set the uniforms
 * using setUniform(). The current shader can be unbound by calling the
 * static unbind() method.
 */
class __Shader {
protected:
	// source code of the vertex and fragment shader
	GLchar *m_vertexSource, *m_fragmentSource;

	// opengl handles
	GLuint m_vertexObject, m_fragmentObject;
	GLuint m_programObject;

	// returns the info (error) log
	void getInfoLog(GLuint object);
public:
	__Shader(GLchar* vertexSource, GLchar* fragmentSource);
	virtual ~__Shader();

	bool compile();

	void bind();
	static void unbind();

	void setUniform4fv(const char* uniform, GLfloat* data);
	void setUniform1f(const char* uniform, GLfloat data);
	void setUniform1i(const char* uniform, GLint data);

	static Shader load(std::string vertexFile, std::string fragmentFile);
};

/**
 * A manager that contains ShaderPtr smart pointers associated with
 * a unique name.
 *
 * All shaders in a folder can be loaded and compiled using the load
 * method.
 */
class ShaderMgr : public std::map<std::string, Shader> {
private:
	static ShaderMgr* s_instance;
	ShaderMgr();
	ShaderMgr(const ShaderMgr& other);
	virtual ~ShaderMgr();
protected:
public:
	static ShaderMgr& instance();
	static void destroy();

	/** Adds the pair of name and shader to the manager. */
	Shader add(std::string name, Shader shader);

	/** Loads all shaders in the specified folder. Vertex (*.vs) and
	 * fragment shader (*.fs) files with the same name are linked together
	 * and stored with the file name as a key.
	 */
	unsigned load(std::string folder);

	/** Returns the associated shader or an empty smart pointer */
	Shader get(std::string name);
};


inline
void __Shader::bind()
{
	glUseProgram(m_programObject);
}

inline
void __Shader::unbind()
{
	glUseProgram(0);
}

inline
void __Shader::setUniform4fv(const char* uniform, GLfloat* data)
{
	glUniform4fv(glGetUniformLocation(m_programObject, uniform), 1, data);
}

inline
void __Shader::setUniform1f(const char* uniform, GLfloat data)
{
	glUniform1f(glGetUniformLocation(m_programObject, uniform), data);
}

inline
void __Shader::setUniform1i(const char* uniform, GLint data)
{
	glUniform1i(glGetUniformLocation(m_programObject, uniform), data);
}


}

#endif /* SHADER_HPP_ */
