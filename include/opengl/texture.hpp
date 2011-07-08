/**
 * @author Markus Doellinger, Robert Waury
 * @date May 21, 2011
 * @file opengl/texture.hpp
 */


#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <GL/glew.h>
#include <boost/tr1/memory.hpp>
#include <string>
#include <map>

namespace ogl {

class __Texture;
typedef std::tr1::shared_ptr<__Texture> Texture;

class __Texture {
public:
	__Texture(GLuint textureID, GLuint stage);
	virtual ~__Texture();

	void setTexParameterf(GLenum pname, GLfloat param);
	void setTexParameteri(GLenum pname, GLint param);
	void setTexParameterfv(GLenum pname, const GLfloat* param);
	void setTexParameteriv(GLenum pname, const GLint* param);

	void setFilter(GLint min, GLint mag);
	void setWrap(GLint s, GLint t);

	void bind();
	void unbind();

	static void stage(GLuint stage);

	static Texture load(std::string file, GLuint target = GL_TEXTURE_2D);
	static Texture create(GLuint target = GL_TEXTURE_2D);

	GLuint m_textureID;
	GLuint m_target;
};


class TextureMgr : public std::map<std::string, Texture> {
private:
	static TextureMgr* s_instance;
	TextureMgr();
	TextureMgr(const TextureMgr& other);
	virtual ~TextureMgr();
protected:
public:
	static TextureMgr& instance();
	static void destroy();

	Texture add(std::string name, Texture texture);
	unsigned load(std::string folder);
	Texture get(std::string name);
};


inline
void __Texture::setTexParameterf(GLenum pname, GLfloat param)
{
	glTexParameterf(m_target, pname, param);
}

inline
void __Texture::setTexParameteri(GLenum pname, GLint param)
{
	glTexParameteri(m_target, pname, param);
}

inline
void __Texture::setTexParameterfv(GLenum pname, const GLfloat* param)
{
	glTexParameterfv(m_target, pname, param);
}

inline
void __Texture::setTexParameteriv(GLenum pname, const GLint* param)
{
	glTexParameteriv(m_target, pname, param);
}

inline
void __Texture::setFilter(GLint min, GLint mag)
{
	setTexParameteri(GL_TEXTURE_MIN_FILTER, min);
	setTexParameterf(GL_TEXTURE_MAG_FILTER, mag);
}

inline
void __Texture::setWrap(GLint s, GLint t)
{
	setTexParameteri(GL_TEXTURE_WRAP_S, s);
	setTexParameteri(GL_TEXTURE_WRAP_T, t);
}

inline
void __Texture::bind()
{
	glBindTexture(m_target, m_textureID);
}

inline
void __Texture::unbind()
{
	glBindTexture(m_target, 0);
}


inline
void __Texture::stage(GLuint stage)
{
	glActiveTexture(stage);
}

inline
TextureMgr& TextureMgr::instance()
{
	if (!s_instance)
		s_instance = new TextureMgr();
	return *s_instance;
}

}

#endif /* TEXTURE_HPP_ */
