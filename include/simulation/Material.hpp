/*
 * Material.hpp
 *
 *  Created on: May 24, 2011
 *      Author: Markus Doellinger
 */

#ifndef MATERIAL_HPP_
#define MATERIAL_HPP_

#include <m3d/m3d.hpp>
#include <Newton.h>
#include <map>
#include <string>
#include <list>

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

	//TODO: add sounds

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

/**
 * The material manager is a container for all materials and
 * material interactions. It maps the material names to the
 * materials and provides for each pair of materials an
 * interaction.
 */
class MaterialMgr {
protected:
	/** The names of materials mapped to the material struct */
	std::map<std::string, Material> m_materials;

	/** For each pair of material ids, there is a material interaction */
	std::map<std::pair<int, int>, MaterialPair> m_pairs;

public:
	MaterialMgr();
	virtual ~MaterialMgr();

	/**
	 * Adds a material to the internal material map and returns the
	 * name of the material.
	 *
	 * @param mat The material to add
	 * @return	  The name of the material
	 */
	std::string add(const Material& mat);

	/**
	 * Removes the material with the given name and all associated
	 * material pairs.
	 *
	 * @param name The name of the material to remove
	 */
	void remove(const std::string& name);

	/**
	 * Removes all materials and material interactions.
	 *
	 * @param addDefault If true, adds a default interaction.
	 */
	void clear(bool addDefault = false);

	/**
	 * Returns the id of the given material name by calculating
	 * a hash value.
	 *
	 * @param name The material name to get the id from
	 * @return     The id of the material name
	 */
	int getID(const std::string& name) const;

	/**
	 * Returns a pointer to the material with the given id, or
	 * NULL, if there is none.
	 *
	 * @param id The id of the material
	 * @return   A pointer to the material or NULL
	 */
	Material* fromID(int id);

	/**
	 * Adds a new material pair to the internal map and returns
	 * the corresponding pair of material ids.
	 *
	 * @param mat0			  The first material
	 * @param mat1			  The second material
	 * @param elasticity	  The elasticity
	 * @param staticFriction  The static friction
	 * @param kineticFriction The kinetic friction
	 * @param softness		  The softness
	 * @return The corresponding pair of material ids
	 */
	std::pair<int, int> addPair(const std::string& mat0,
								const std::string& mat1,
								float elasticity,
								float staticFriction,
								float kineticFriction,
								float softness);

	/**
	 * Retrieves the material pair of the given material ids. If
	 * there is none, returns the default pair.
	 *
	 * @param id0 The first id
	 * @param id1 The second id
	 * @return The material pair or the default pair
	 */
	MaterialPair& getPair(int id0, int id1);

	/**
	 * The Newton callback called when a contact is being resolved.
	 *
	 * @param contactJoint The contact constraint
	 * @param timestep	   The timestep of NewtonUpdate
	 * @param threadIndex  The index of the executing thread
	 */
	void processContact(const NewtonJoint* contactJoint,
						dFloat timestep,
						int threadIndex);

	//load
	//save
};

}

#endif /* MATERIAL_HPP_ */
