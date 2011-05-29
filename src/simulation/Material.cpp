/*
 * Material.cpp
 *
 *  Created on: May 24, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Material.hpp>
#include <limits.h>
#ifdef _WIN32
#include <boost/functional/hash.hpp>
#else
#include <tr1/functional_hash.h>
#endif
#include <Newton.h>
#include <xml/rapidxml.hpp>
#include <xml/rapidxml_utils.hpp>

namespace sim {

MaterialMgr* MaterialMgr::s_instance = NULL;

Material::Material(const std::string& name)
		: name(name)
{
}

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


void Material::load(rapidxml::xml_node<>* const node)
{
	using namespace rapidxml;
	for (xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
		std::string attribute = attr->name();
		if( attribute == "name" )		name = attr->value();
		if( attribute == "texture" )		texture = attr->value();
		if( attribute == "shader" )		shader = attr->value();
		if( attribute == "ambient" )		ambient.assign(attr->value());
		if( attribute == "diffuse")		diffuse.assign(attr->value());
		if( attribute == "specular")		specular.assign(attr->value());
		if( attribute == "shininess")	shininess = atof(attr->value());
		}
}

/*
void Material::save(XMLNode) const;
{
	// save properties
	// vectors can be saved to a string like that:
	//	std::string str = diffuse.str()
}
*/

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



void MaterialPair::load(rapidxml::xml_node<>* node)
{
	using namespace rapidxml;
	MaterialMgr& m = MaterialMgr::instance();
	for (xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
		std::string attribute = attr->name();
		if( attribute == "mat0" )			id0 = m.getID(attr->value());
		if( attribute == "mat1" )			id1 = m.getID(attr->value());
		if( attribute == "elasticity" )		elasticity = atof(attr->value());
		if( attribute == "staticFriction" )	staticFriction = atof(attr->value());
		if( attribute == "kineticFriction")	kineticFriction = atof(attr->value());
		if( attribute == "softness")			softness = atof(attr->value());
	}
}

/*
void MaterialPair::save(XMLNode) const;
{
	// save properties
	// get material by using MaterialMgr::fromID
	// and then get the name from it
	// MaterialMgr::instance().fromID(id)->name;
}
*/



MaterialMgr::MaterialMgr()
{
	clear(true);
}

MaterialMgr::~MaterialMgr()
{
	clear();
}

MaterialMgr::MaterialMgr(const MaterialMgr& other)
{
}


MaterialMgr& MaterialMgr::instance()
{
	if (!s_instance)
		s_instance = new MaterialMgr();
	return *s_instance;
}

void MaterialMgr::destroy()
{
	if (s_instance)
		delete s_instance;
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



Material* MaterialMgr::fromID(unsigned int id)
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


bool MaterialMgr::load(const char* fileName)
{
	using namespace rapidxml;

	try {
		file<char> f(fileName);
		char* m = f.data();
	
		// string containing valid xml for test purposes
		// char m[] = "<?xml version=\"1.0\" ?><materials><material name=\"wood_shiny\" texture=\"wood_shiny\" shader=\"ppl_textured\" ambient=\"1, 1, 1, 1\" diffuse=\"1, 1, 1, 1\" specular=\"0.6, 0.6, 0.6, 1\" shininess=\"50\" /><material name=\"wood_matt\" texture=\"wood_matt\" shader=\"ppl_textured\" ambient=\"1, 1, 1, 1\" diffuse=\"1, 1, 1, 1\" specular=\"0.1, 0.1, 0.1, 1\" shininess=\"20\" /><pair mat0=\"wood_shiny\" mat1=\"wood_shiny\" elasticity=\"0.100000\" staticFriction=\"0.450000\" kineticFriction=\"0.310000\" softness=\"0.050000\" /><pair mat0=\"wood_matt\" mat1=\"wood_matt\" elasticity=\"0.15000\" staticFriction=\"0.55000\" kineticFriction=\"0.45000\" softness=\"0.080000\" /></materials>";

		xml_document<> materials;

	
		materials.parse<0>(m);
	

		// this is important so we don't parse the materials tag but the material and pair tags
		xml_node<>* nodes = materials.first_node();

		for (xml_node<>* node = nodes->first_node(); node; node = node->next_sibling()) {
			std::string name = node->name();
			if(name == "material") {
				Material mat("none");
				mat.load(node);
				add(mat);
			}
			if(name == "pair") {
				MaterialPair pair;
				pair.load(node);
				m_pairs[std::make_pair(pair.id0, pair.id1)] = pair;
			}
		}
	

		// foreach material tag
			// Material mat("none");
			// mat.load(elem);
			// add(mat);

		// foreach pair tag
			// MaterialPair pair;
			// pair.load(elem);
			// m_pairs[std::make_pair(pair.id0, pair.id1)] = pair;

		return true;
	}
	// catch all exceptions
	catch (...) {
		return false;
	}
}

bool MaterialMgr::save(const char* fileName)
{

	// create document
	// create root element "materials"

	// foreach material
		// mat.save(root)

	// foreach pair
		// pair.save(root)

	// save document

	return true;
}


MaterialPair& MaterialMgr::getPair(int id0, int id1)
{
	if (id0 > id1) {
		int tmp = id0;
		id0 = id1;
		id1 = tmp;
	}
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
