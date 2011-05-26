/*
 * Texture.hpp
 *
 *  Created on: May 21, 2011
 *      Author: Markus Doellinger, Robert Waury
 */

#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <GL/glew.h>
#ifdef _WIN32
#include <boost/tr1/memory.hpp>
#else
#include <tr1/memory>
#endif
#include <string>
#include <map>

namespace ogl {

class Texture;
typedef std::tr1::shared_ptr<Texture> TexturePtr;

class Texture {
public:
	Texture(GLuint textureID, GLuint stage);
	virtual ~Texture();

	void setTexParameterf(GLenum pname, GLfloat param);
	void setTexParameteri(GLenum pname, GLint param);
	void setTexParameterfv(GLenum pname, const GLfloat* param);
	void setTexParameteriv(GLenum pname, const GLint* param);

	void setFilter(GLint min, GLint mag);
	void setWrap(GLint s, GLint t);

	void bind();
	void unbind();

	static void stage(GLuint stage);

	static TexturePtr load(std::string file, GLuint target = GL_TEXTURE_2D);
	static TexturePtr create(GLuint target = GL_TEXTURE_2D);

	GLuint m_textureID;
	GLuint m_target;
};


class TextureMgr : public std::map<std::string, TexturePtr> {
private:
	static TextureMgr* s_instance;
	TextureMgr();
	TextureMgr(const TextureMgr& other);
	virtual ~TextureMgr();
protected:
public:
	static TextureMgr& instance();
	static void destroy();

	TexturePtr add(std::string name, TexturePtr texture);
	unsigned load(std::string folder);
	TexturePtr get(std::string name);
};


inline
void Texture::setTexParameterf(GLenum pname, GLfloat param)
{
	glTexParameterf(m_target, pname, param);
}

inline
void Texture::setTexParameteri(GLenum pname, GLint param)
{
	glTexParameteri(m_target, pname, param);
}

inline
void Texture::setTexParameterfv(GLenum pname, const GLfloat* param)
{
	glTexParameterfv(m_target, pname, param);
}

inline
void Texture::setTexParameteriv(GLenum pname, const GLint* param)
{
	glTexParameteriv(m_target, pname, param);
}

inline
void Texture::setFilter(GLint min, GLint mag)
{
	setTexParameteri(GL_TEXTURE_MIN_FILTER, min);
	setTexParameterf(GL_TEXTURE_MAG_FILTER, mag);
}

inline
void Texture::setWrap(GLint s, GLint t)
{
	setTexParameteri(GL_TEXTURE_WRAP_S, s);
	setTexParameteri(GL_TEXTURE_WRAP_T, t);
}

inline
void Texture::bind()
{
	glBindTexture(m_target, m_textureID);
}

inline
void Texture::unbind()
{
	glBindTexture(m_target, 0);
}
;

inline
void Texture::stage(GLuint stage)
{
	glActiveTexture(stage);
}


}

#endif /* TEXTURE_HPP_ */
