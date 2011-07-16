/**
 * @author Markus Doellinger, Robert Waury
 * @date May 24, 2011
 * @file simulation/material.hpp
 */

#ifndef MATERIAL_HPP_
#define MATERIAL_HPP_

#include <m3d/m3d.hpp>
#include <Newton.h>
#include <map>
#include <string>
#include <list>
#include <set>
#include <xml/rapidxml.hpp>


#define MAX_SOUND_DISTANCE 500

namespace sim {

/**
 * A material for graphics and physical properties.
 */
struct Material {
	/** The name of the material */
	std::string name;

	/** The texture of the material, i.e. the key in the TextureMgr */
	std::string texture;

	/** Another texture of the material, e.g. a normal map */
	std::string texture1;

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


	Material(const std::string& name);
	/**
	 * The copy constructor
	 *
	 * @param m The material to copy from.
	 */
	Material(const Material& m);

	/**
	 *	Loads material from XML node
	 *
	 *	@param	node	Pointer to current XML material node
	 *	@throws	rapidxml::parse_error	attribute not found
	 */
	void load(rapidxml::xml_node<>* node);

	/**
	 *	Saves material to XML node
	 *
	 *	@param	materials	Pointer to empty XML root node
	 *	@param	doc			Pointer to XML document
	 */
	void save(rapidxml::xml_node<>* materials, rapidxml::xml_document<>* doc) const;
};

/**
 * A pair of materials is used to define the physical interactions.
 */
struct MaterialPair {
	/** The id of the two materials */
	int mat0, mat1;

	float elasticity;
	float staticFriction;
	float kineticFriction;
	float softness;

	std::string impactSound;

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

	/**
	 *	Loads material pair from XML node
	 *
	 *	@param	node	Pointer to current XML material pair node
	 *	@throws	rapidxml::parse_error	attribute not found
	 */
	void load(rapidxml::xml_node<>* node);

	/**
	 *	Saves material pair to XML node
	 *
	 *	@param	materials	Pointer to XML root node
	 *	@param	doc			Pointer to XML document
	 */
	void save(rapidxml::xml_node<>* materials, rapidxml::xml_document<>* doc) const;
};

/**
 * The material manager is a container for all materials and
 * material interactions. It maps the material names to the
 * materials and provides for each pair of materials an
 * interaction.
 */
class MaterialMgr {
private:
	// singleton
	static MaterialMgr* s_instance;
	MaterialMgr();
	MaterialMgr(const MaterialMgr& other);
	virtual ~MaterialMgr();

protected:
	/** The names of materials mapped to the material struct */
	std::map<std::string, Material> m_materials;

	/** For each pair of material ids, there is a material interaction */
	std::map<std::pair<int, int>, MaterialPair> m_pairs;

public:
	/**
	 * Returns an instance of the MaterialMgr and creates it,
	 * if there is none.
	 *
	 * @return The MaterialMgr
	 */
	static MaterialMgr& instance();

	/**
	 * Destroys the instance of the MaterialMgr
	 */
	static void destroy();

	/**
	 * Stores all materials in the given set and returns the number
	 * of materials stored in it. Usage:
	 *
	 * #include <set>
	 * std::set<std::string> materials;
	 * MaterialMgr::instance().getMaterials(materials);
	 *
	 * @param materials The set in which the materials should be stored
	 * @return          The number of materials stored in the set
	 */
	int getMaterials(std::set<std::string>& materials);

	/**
	 * Applies the given material. If it is not available, disable all
	 * material properties.
	 *
	 * @param material
	 */
	void applyMaterial(const std::string& material);

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
	Material* fromID(unsigned int id);

	/**
	 * Returns the material with the given name, or NULL.
	 *
	 * @param name
	 * @return
	 */
	Material* get(const std::string& name);

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
	 * Saves all material and pairs to the given XML material file.
	 *
	 * @param  fileName The XML material file.
	 * @return True, if successfull, false otherwise
	 */
	bool save(const std::string& fileName);

	/**
	 * Loads all material and pairs from the given XML material file.
	 *
	 * @param fileName The XML material file.
	 * @return True, if successfull, false otherwise
	 */
	bool load(const std::string& fileName);

	/**
	 * Retrieves the material pair of the given material ids. If
	 * there is none, returns the default pair.
	 *
	 * @param mat0 The first id
	 * @param mat1 The second id
	 * @return The material pair or the default pair
	 */
	MaterialPair& getPair(int mat0, int mat1);

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

	static void	GenericContactCallback(const NewtonJoint* contactJoint,
			dFloat timestep,
			int threadIndex);
};


inline MaterialMgr& MaterialMgr::instance()
{
	if (!s_instance)
		s_instance = new MaterialMgr();
	return *s_instance;
}


}

#endif /* MATERIAL_HPP_ */
