/*
 * Skydome.hpp
 *
 *  Created on: Jun 19, 2011
 *      Author: markus
 */

#ifndef SKYDOME_HPP_
#define SKYDOME_HPP_

#include <string>
#include <m3d/m3d.hpp>
#include <opengl/texture.hpp>
#include <opengl/shader.hpp>

namespace ogl {

using namespace m3d;

class Skydome {
protected:
	GLuint m_list;
	Shader m_shader;
	Texture m_texture;
	float m_radius;
	Vec4f m_color;
	float m_time;
public:
	Skydome();
	Skydome(const std::string& texture, const std::string& shader, const std::string& fileName);
	virtual ~Skydome();

	void setRadius(float radius) { m_radius = radius * 0.01f; };

	void load(const std::string& texture, const std::string& shader, const std::string& fileName);
	void clear();

	void update(float dt);
	void render();
};

}

#endif /* SKYDOME_HPP_ */
