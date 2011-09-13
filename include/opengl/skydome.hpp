/**
 * @author Markus Doellinger
 * @date Jun 23, 2011
 * @file opengl/skydome.hpp
 */

#ifndef SKYDOME_HPP_
#define SKYDOME_HPP_

#include <string>
#include <m3d/m3d.hpp>
#include <opengl/texture.hpp>
#include <opengl/shader.hpp>
#include <opengl/camera.hpp>

namespace ogl {

using namespace m3d;

/**
 * A fancy skydome with dynamic clouds and a sun with optional lens flares.
 */
class Skydome {
protected:
	GLuint m_flares;
	GLuint m_list;
	Shader m_shader;
	GLuint m_clouds;
	float m_radius;
	Vec4f m_horizon;
	float m_time, m_delta;
	float m_fadeTime;
public:
	typedef enum { BIG_GLOW = 0, GLOW, HALO, STREAK } Flares;

	Skydome();
	Skydome(float radius, const std::string& clouds, const std::string& shader,
			const std::string& fileName, const std::string& flares);
	virtual ~Skydome();

	void load(float radius, const std::string& clouds, const std::string& shader,
			const std::string& fileName, const std::string& flares);
	void clear();

	void update(float dt);
	void render(const Camera& cam, const Vec3f& light, bool concealed);
};

}

#endif /* SKYDOME_HPP_ */
