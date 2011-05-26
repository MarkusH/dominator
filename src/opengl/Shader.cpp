/*
 * Shader.cpp
 *
 *  Created on: May 21, 2011
 *      Author: Markus Doellinger, Robert Waury
 */

#include <opengl/Shader.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <cstring>
#include <string>
#ifdef _WIN32
#define BOOST_FILESYSTEM_VERSION 2
#include <boost/filesystem.hpp>
#else
#include <dirent.h>
#endif
#include <stdexcept>

namespace ogl {

ShaderMgr* ShaderMgr::s_instance = NULL;

Shader::Shader(GLchar* vertexSource, GLchar* fragmentSource) :
	m_vertexSource(vertexSource), m_fragmentSource(fragmentSource) {
	m_programObject = 0;
	m_vertexObject = 0;
	m_fragmentObject = 0;
}

Shader::~Shader() {
#ifdef _DEBUG
	std::cout << "delete shader" << std::endl;
#endif
	if (m_vertexSource) delete m_vertexSource;
	if (m_fragmentSource) delete m_fragmentSource;
	if (m_programObject) glDeleteProgram(m_programObject);
}

void Shader::getInfoLog(GLuint object)
{
	GLint len, slen;
	glGetObjectParameterivARB(object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &len);
	if (len > 1) {
		GLcharARB* log = new GLcharARB[len];
		glGetInfoLogARB(object, len, &slen, log);
		std::cout << log << std::endl;
		delete log;
	}
}

bool Shader::compile()
{
	// load vertex source
	m_vertexObject = glCreateShader(GL_VERTEX_SHADER);
	int len = strlen(m_vertexSource);
	glShaderSource(m_vertexObject, 1, (const GLchar**)&m_vertexSource, &len);
	glCompileShader(m_vertexObject);

	GLint result[3];

	// check if compiled
	glGetObjectParameterivARB(m_vertexObject, GL_COMPILE_STATUS, &result[0]);
	getInfoLog(m_vertexObject);

	// load fragment source
	m_fragmentObject = glCreateShader(GL_FRAGMENT_SHADER);
	len = strlen(m_fragmentSource);
	glShaderSource(m_fragmentObject, 1, (const GLchar**)&m_fragmentSource, &len);
	glCompileShader(m_fragmentObject);

	// check if compiled
	glGetObjectParameterivARB(m_fragmentObject, GL_COMPILE_STATUS, &result[1]);
	getInfoLog(m_fragmentObject);

	// create and link program
	m_programObject = glCreateProgram();

	glAttachShader(m_programObject, m_vertexObject);
	glAttachShader(m_programObject, m_fragmentObject);

	glLinkProgram(m_programObject);

	glGetProgramiv(m_programObject, GL_LINK_STATUS, &result[2]);
	getInfoLog(m_programObject);
	if (!result[0] || !result[1] || !result[2]) return false;

	glDeleteShader(m_vertexObject);
	glDeleteShader(m_fragmentObject);

	return true;
}


static unsigned long getFileLength(std::ifstream& file)
{
    if (!file.good()) return 0;

    unsigned long pos = file.tellg();
    file.seekg(0, std::ios::end);
    unsigned long len = file.tellg();
    file.seekg(pos, std::ios::beg);

    return len;
}

static GLchar* getFileContent(std::string& fileName)
{
	std::ifstream file;
	file.open(fileName.c_str(), std::ios::in);

	if (!file)
		return NULL;

	int len = getFileLength(file);

	if (len == 0)
		return NULL;

	GLchar* source = (GLchar*) new char[len + 1];
	if (source == 0)
		return NULL;

	source[len] = 0;

	unsigned int i = 0;
	while (file.good()) {
		source[i] = file.get();
		if (!file.eof())
			i++;
	}

	source[i] = 0;

	file.close();

	return source;
}


ShaderPtr Shader::load(std::string vertexFile, std::string fragmentFile)
{
	GLchar* vsrc = getFileContent(vertexFile);
	if (!vsrc) return ShaderPtr();

	GLchar* fsrc = getFileContent(fragmentFile);
	if (!fsrc) return ShaderPtr();

	return ShaderPtr(new Shader(vsrc, fsrc));
}


ShaderMgr::ShaderMgr()
	: std::map<std::string, ShaderPtr>()
{
}

ShaderMgr::ShaderMgr(const ShaderMgr& other)
{
}

ShaderMgr::~ShaderMgr()
{
}

ShaderMgr& ShaderMgr::instance()
{
	if (!s_instance)
		s_instance = new ShaderMgr();
	return *s_instance;
}

void ShaderMgr::destroy()
{
	if (s_instance)
		delete s_instance;
}

ShaderPtr ShaderMgr::add(std::string name, ShaderPtr shader)
{
	(*this)[name] = shader;
	return shader;
}

unsigned ShaderMgr::load(std::string folder)
{
	int count = 0;
#ifdef _WIN32
	using namespace boost::filesystem;

	path p (folder);
	
	if(is_directory(p)) {
		if(!is_empty(p)) {
			directory_iterator end_itr;
			for(directory_iterator itr(p); itr != end_itr; ++itr) {
				if(itr->leaf().find(".vs") != std::string::npos) {
					std::string vs = itr->string();
					std::string fs = vs;
					fs.replace(vs.size() - 2, 1, "f");
					ShaderPtr shader = Shader::load(vs, fs);
					shader->compile();
					add(basename(*itr), shader);
				}
			}
		}

	}
#else
	if (folder.at(folder.size() - 1) != '/')
		folder.append("/");

	DIR* dir = opendir(folder.c_str());
	dirent* ent;

	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string vs(ent->d_name);
			if (vs.find(".vs") != std::string::npos) {
				std::string fs(vs);
				fs.replace(fs.size() - 2, 1, "f");
				ShaderPtr shader = Shader::load(folder + vs, folder + fs);
				shader->compile();
				std::string name(vs);
				name.erase(name.size() - 3, 3);
				add(name, shader);
			}
		}
		closedir(dir);
	}
#endif
	return count;
}

ShaderPtr ShaderMgr::get(std::string name)
{
	ShaderMgr::iterator it = this->find(name);
	if (it == this->end()) {
		ShaderPtr result;
		return result;
	}
	return it->second;
}


}
