/*
 * Material.hpp
 *
 *  Created on: May 24, 2011
 *      Author: markus
 */

#ifndef MATERIAL_HPP_
#define MATERIAL_HPP_

#include <m3d/m3d.hpp>

namespace sim {

/**
 * A material for graphics and physical properties.
 */
struct Material {
	/** The name of the material */
	std::string name;

	/** The texture of the material, i.e. the key in the TextureMgr */
	std::string texture;

	/** The shader of the material, i.e. the key in the ShaderMgr */
	std::string shader;

	/** The ambient material color */
	m3d::Vec4f ambient;

	/** The diffuse material color */
	m3d::Vec4f diffuse;

	/** The ambient specular color */
	m3d::Vec4f specular;

	/** The shininess of the material */
	float shininess;

	/**
	 * The copy constructor
	 *
	 * @param m The material to copy from.
	 */
	Material(const Material& m);

	//load(node);
	//save(node);
};

/**
 * A pair of materials is used to define the physical interactions.
 */
struct MaterialPair {
	/** The id of the two materials */
	int id0, id1;

	float elasticity;
	float staticFriction;
	float kineticFriction;
	float softness;

	/**
	 * Constructs the default material.
	 */
	MaterialPair();

	/**
	 * The copy constructor.
	 *
	 * @param p The pair to copy from.
	 */
	MaterialPair(const MaterialPair& p);

	//load(node);
	//save(node);
};

}

#endif /* MATERIAL_HPP_ */
