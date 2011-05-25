/*
 * Material.cpp
 *
 *  Created on: May 24, 2011
 *      Author: markus
 */

#include <simulation/Material.hpp>
#include <limits.h>
#include <tr1/functional_hash.h>
#include <Newton.h>

namespace sim {

Material::Material(const Material& m)
	: name(m.name),
	  texture(m.texture),
	  shader(m.shader),
	  ambient(m.ambient),
	  diffuse(m.diffuse),
	  specular(m.specular),
	  shininess(m.shininess)
{
}

MaterialPair::MaterialPair()
{
	id0 = id1 = 0;
	elasticity = 0.4f;
	staticFriction = 0.9f;
	kineticFriction = 0.5f;
	softness = 0.1f;
}

MaterialPair::MaterialPair(const MaterialPair& p)
	: id0(p.id0),
	  id1(p.id1),
	  elasticity(p.elasticity),
	  staticFriction(p.staticFriction),
	  kineticFriction(p.kineticFriction),
	  softness(p.softness)
{
}



MaterialMgr::MaterialMgr()
{
	clear(true);
}

MaterialMgr::~MaterialMgr()
{
	clear();
}


std::string MaterialMgr::add(const Material& mat)
{
	m_materials.insert(std::make_pair(mat.name, mat));
	return mat.name;
}

void MaterialMgr::remove(const std::string& name)
{
	int id = getID(name);
	if (id == 0)
		return;

	std::map<std::pair<int, int>, MaterialPair>::iterator it;
	for (it = m_pairs.begin(); it != m_pairs.end(); ) {
		if (it->first.first == id || it->first.second == id)
			m_pairs.erase(it++);
		else
			++it;
	}
	m_materials.erase(name);
}



void MaterialMgr::clear(bool addDefault)
{
	m_pairs.clear();
	m_materials.clear();
	if (addDefault) {
		MaterialPair pair;
		m_pairs[std::make_pair(0, 0)] = pair;
	}
}



int MaterialMgr::getID(const std::string& name) const
{
	if (name.size() == 0)
		return 0;
	std::tr1::hash<std::string> string_hash;
	return string_hash(name) % INT_MAX;
}



Material* MaterialMgr::fromID(int id)
{
	if (id == 0)
		return NULL;

	id %= INT_MAX;


	std::map<std::string, Material>::iterator it;
	for (it = m_materials.begin(); it != m_materials.end(); ++it) {
		std::tr1::hash<std::string> string_hash;
		if (string_hash(it->first) % INT_MAX == id)
			return &(it->second);
	}
	return NULL;
}



std::pair<int,int> MaterialMgr::addPair(const std::string& mat0,
										const std::string & mat1,
										float elasticity,
										float staticFriction,
										float kineticFriction,
										float softness)
{
	MaterialPair pair;
	pair.id0 = getID(mat0);
	pair.id1 = getID(mat1);
	pair.elasticity = elasticity;
	pair.staticFriction = staticFriction;
	pair.kineticFriction = kineticFriction;
	pair.softness = softness;
	m_pairs[std::make_pair(pair.id0, pair.id1)] = pair;

	return std::make_pair(pair.id0, pair.id1);
}

MaterialPair& MaterialMgr::getPair(int id0, int id1)
{
	// search for interaction
	std::map<std::pair<int, int>, MaterialPair>::iterator it;
	it = m_pairs.find(std::make_pair(id0, id1));

	if (it == m_pairs.end())
		// return default pair
		return m_pairs.find(std::make_pair(0, 0))->second;
	else
		// return found pair
		return it->second;
}


void MaterialMgr::processContact(const NewtonJoint* contactJoint, float timestep, int threadIndex)
{
	int id0, id1;

	// get the contact material
	NewtonMaterial* material;
	const NewtonBody *body0, *body1;

	body0 = NewtonJointGetBody0(contactJoint);
	body1 = NewtonJointGetBody1(contactJoint);
	for (void* contact = NewtonContactJointGetFirstContact(contactJoint);
				contact; contact = NewtonContactJointGetNextContact(contactJoint, contact)) {


		// get the material of this contact;
		material = NewtonContactGetMaterial(contact);

		// get the first id from the body
		id0 = NewtonMaterialGetBodyCollisionID(material, body0);
		if (id0 == 0)
			id0	= NewtonMaterialGetContactFaceAttribute(material);

		// get the second id
		id1 = NewtonMaterialGetBodyCollisionID(material, body1);
		if (id1 == 0)
			id1	= NewtonMaterialGetContactFaceAttribute(material);

		// get the pair for the materials, or the default pair
		MaterialPair& pair = getPair(id0, id1);

		// set the material properties for this contact
		NewtonMaterialSetContactElasticity(material, pair.elasticity);
		NewtonMaterialSetContactSoftness(material, pair.softness);
		NewtonMaterialSetContactFrictionCoef(material, pair.staticFriction, pair.kineticFriction, 0);
		NewtonMaterialSetContactFrictionCoef(material, pair.staticFriction, pair.kineticFriction, 1);

		//TODO: get impact information and play a sound
	}


}




}
