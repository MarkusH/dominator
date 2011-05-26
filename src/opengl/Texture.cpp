/*
 * Texture.cpp
 *
 *  Created on: May 21, 2011
 *      Author: markus
 */

#include <opengl/Texture.hpp>
#include "stb_image.hpp"
#ifdef _WIN32
	// TODO: useful include
#else
#include <dirent.h>
#endif
#include <stdexcept>
#include <iostream>

namespace ogl {

TextureMgr* TextureMgr::s_instance = NULL;

Texture::Texture(GLuint textureID, GLuint target)
	: m_textureID(textureID), m_target(target)
{
}

Texture::~Texture()
{
#ifdef _DEBUG
	std::cout << "delete texture" << std::endl;
#endif
	if (glIsTexture(m_textureID))
		glDeleteTextures(0, &m_textureID);
}

TexturePtr Texture::load(std::string file, GLuint target)
{
	int w, h, c;
	unsigned char* data = stbi_load(file.c_str(), &w, &h, &c, STBI_rgb_alpha);

	if (!data)
		return TexturePtr();

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(target, textureID);

    //glTexImage2D(stage, 0, GL_RGBA, Image.GetWidth(), Image.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, Image.GetPixelsPtr());
    //gluBuild2DMipmaps(target, GL_RGBA, Image.GetWidth(), Image.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, Image.GetPixelsPtr());
    gluBuild2DMipmaps(target, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    TexturePtr result(new Texture(textureID, target));
    return result;
}

TexturePtr Texture::create(GLuint target)
{
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(target, textureID);

    TexturePtr result(new Texture(textureID, target));
    return result;
}

TextureMgr::TextureMgr()
	: std::map<std::string, TexturePtr>()
{
}

TextureMgr::TextureMgr(const TextureMgr& other)
{
}

TextureMgr::~TextureMgr()
{
}

TextureMgr& TextureMgr::instance()
{
	if (!s_instance)
		s_instance = new TextureMgr();
	return *s_instance;
}

void TextureMgr::destroy()
{
	if (s_instance)
		delete s_instance;
}

TexturePtr TextureMgr::add(std::string name, TexturePtr texture)
{
	(*this)[name] = texture;
	return texture;
}

unsigned TextureMgr::load(std::string folder)
{
	int count = 0;
#ifdef _WIN32
	// open folder
	// iterate over the folder
	// find *.[a-zA-Z] files
	// load with Texture Manager
	// close folder
#else
	if (folder.at(folder.size() - 1) != '/')
		folder.append("/");

	DIR* dir = opendir(folder.c_str());
	dirent* ent;

	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string file(ent->d_name);
			if (file.size() > 3) {
				std::string name = file.substr(0, file.rfind(".", file.size()));
#ifdef _DEBUG
				std::cout << "load texture " << name << std::endl;
#endif
				TexturePtr texture = Texture::load(folder + file, GL_TEXTURE_2D);
				add(name, texture);
			}
		}
		closedir(dir);
	}
#endif
	return count;
}

TexturePtr TextureMgr::get(std::string name)
{
	TextureMgr::iterator it = this->find(name);
	if (it == this->end()) {
		TexturePtr result;
		return result;
	}
	return it->second;

}

}
