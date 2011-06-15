/*
 * Material.cpp
 *
 *  Created on: May 24, 2011
 *      Author: Markus Doellinger
 */

#include <simulation/Material.hpp>
#include <opengl/Texture.hpp>
#include <opengl/Shader.hpp>
#include <GL/glew.h>
#include <limits.h>
#ifdef _WIN32
#include <boost/functional/hash.hpp>
#else
#include <tr1/functional_hash.h>
#endif
#include <Newton.h>
#include <xml/rapidxml_utils.hpp>
#include <xml/rapidxml_print.hpp>
#include <fstream> // for file I/O
#include <string.h>
#include <util/toString.hpp>

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

	xml_attribute<> *attr = node->first_attribute();
	
	name = attr->value();

	attr = attr->next_attribute();
	texture = attr->value();

	attr = attr->next_attribute();
	shader = attr->value();

	attr = attr->next_attribute();
	ambient.assign(attr->value());

	attr = attr->next_attribute();
	diffuse.assign(attr->value());

	attr = attr->next_attribute();
	specular.assign(attr->value());
	
	attr = attr->next_attribute();
	shininess = atof(attr->value());

}


void Material::save(rapidxml::xml_node<>* materials, rapidxml::xml_document<>* doc) const
{
	using namespace rapidxml;

	// create material node
	xml_node<>* node = doc->allocate_node(node_element, "material");
	materials->append_node(node);

	// allocate string for name and create attribute
	char* pName = doc->allocate_string(name.c_str());
	xml_attribute<>* attrN = doc->allocate_attribute("name", pName);
	node->append_attribute(attrN);

	// allocate string for texture and create attribute
	char* pTexture = doc->allocate_string(texture.c_str());
	xml_attribute<>* attrT = doc->allocate_attribute("texture", pTexture);
	node->append_attribute(attrT);

	// allocate string for shader and create attribute
	char* pShader = doc->allocate_string(shader.c_str());
	xml_attribute<>* attrS = doc->allocate_attribute("shader", pShader);
	node->append_attribute(attrS);

	// allocate string for ambient and create attribute
	char* pAmbient = doc->allocate_string(ambient.str().c_str());
	xml_attribute<>* attrA = doc->allocate_attribute("ambient", pAmbient);
	node->append_attribute(attrA);

	// allocate string for diffuse and create attribute
	char* pDiffuse = doc->allocate_string(diffuse.str().c_str());
	xml_attribute<>* attrD = doc->allocate_attribute("diffuse", pDiffuse);
	node->append_attribute(attrD);

	// allocate string for specular and create attribute
	char* pSpecular = doc->allocate_string(specular.str().c_str());
	xml_attribute<>* attrSp = doc->allocate_attribute("specular", pSpecular);
	node->append_attribute(attrSp);

	// allocate string for shininess and create attribute
	//char* pShininess = doc->allocate_string(boost::lexical_cast<std::string>(shininess).c_str());
	char* pShininess = doc->allocate_string(util::toString(shininess));
	xml_attribute<>* attrSh = doc->allocate_attribute("shininess", pShininess);
	node->append_attribute(attrSh);
	//free(pShininess);

	
	// save properties
	// vectors can be saved to a string like that:
	//	std::string str = diffuse.str()
}


MaterialPair::MaterialPair()
{
	mat0 = mat1 = 0;
	elasticity = 0.4f;
	staticFriction = 0.9f;
	kineticFriction = 0.5f;
	softness = 0.1f;
}

MaterialPair::MaterialPair(const MaterialPair& p)
	: mat0(p.mat0),
	  mat1(p.mat1),
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
	
	xml_attribute<> *attr = node->first_attribute();

	mat0 = m.getID(attr->value());

	attr = attr->next_attribute();
	mat1 = m.getID(attr->value());

	attr = attr->next_attribute();
	elasticity = atof(attr->value());

	attr = attr->next_attribute();
	staticFriction = atof(attr->value());

	attr = attr->next_attribute();
	kineticFriction = atof(attr->value());

	attr = attr->next_attribute();
	softness = atof(attr->value());

	if (mat0 > mat1) {
		int tmp = mat0;
		mat0 = mat1;
		mat1 = tmp;
	}
}


void MaterialPair::save(rapidxml::xml_node<>* materials, rapidxml::xml_document<>* doc) const
{
	using namespace rapidxml;

	MaterialMgr& m = MaterialMgr::instance();

	// create material node
	xml_node<>* node = doc->allocate_node(node_element, "pair");
	materials->append_node(node);

	// allocate string for mat0 and create attribute
	if (m.fromID(mat0) != NULL) {
		const char* pMat0 = doc->allocate_string(m.fromID(mat0)->name.c_str());
		xml_attribute<>* attrMat0 = doc->allocate_attribute("mat0", pMat0);
		node->append_attribute(attrMat0);
	}
	else {
		const char* pMat0 = doc->allocate_string("");
		xml_attribute<>* attrMat0 = doc->allocate_attribute("mat0", pMat0);
		node->append_attribute(attrMat0);
	}

	// allocate string for mat1 and create attribute
	if (m.fromID(mat1) != NULL) {
		const char* pMat1 = doc->allocate_string(m.fromID(mat1)->name.c_str());
		xml_attribute<>* attrMat1 = doc->allocate_attribute("mat1", pMat1);
		node->append_attribute(attrMat1);
	}
	else {
		const char* pMat1 = doc->allocate_string("");
		xml_attribute<>* attrMat1 = doc->allocate_attribute("mat1", pMat1);
		node->append_attribute(attrMat1);
	}


	// allocate string for elasticity and create attribute
	//char* pElasticity = doc->allocate_string(boost::lexical_cast<std::string>(elasticity).c_str());
	char* pElasticity = doc->allocate_string(util::toString(elasticity));
	xml_attribute<>* attrE = doc->allocate_attribute("elasticity", pElasticity);
	node->append_attribute(attrE);
	//free(pElasticity);

	// allocate string for static friction and create attribute
	//char* pStaticFriction = doc->allocate_string(boost::lexical_cast<std::string>(staticFriction).c_str());
	char* pStaticFriction = doc->allocate_string(util::toString(staticFriction));
	xml_attribute<>* attrSF = doc->allocate_attribute("staticFriction", pStaticFriction);
	node->append_attribute(attrSF);
	//free(pStaticFriction);

	// allocate string for kinetic friction and create attribute
	//char* pKineticFriction = doc->allocate_string(boost::lexical_cast<std::string>(kineticFriction).c_str());
	char* pKineticFriction = doc->allocate_string(util::toString(kineticFriction));
	xml_attribute<>* attrKF = doc->allocate_attribute("kineticFriction", pKineticFriction);
	node->append_attribute(attrKF);
	//free(pKineticFriction);

	// allocate string for softness and create attribute
	//char* pSoftness = doc->allocate_string(boost::lexical_cast<std::string>(softness).c_str());
	char* pSoftness = doc->allocate_string(util::toString(softness));
	xml_attribute<>* attrS = doc->allocate_attribute("softness", pSoftness);
	node->append_attribute(attrS);
	//free(pSoftness);

}




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

void MaterialMgr::destroy()
{
	if (s_instance)
		delete s_instance;
}

void MaterialMgr::applyMaterial(const std::string& material) {
	const Material* const _mat = get(material);
	/// @todo only switch shader/texture if necessary
	if (_mat != NULL) {
		const Material& mat = *_mat;
		ogl::Texture texture = ogl::TextureMgr::instance().get(mat.texture);

		if (texture) {
			glEnable(GL_TEXTURE_2D);
			texture->bind();
		} else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glMaterialfv(GL_FRONT, GL_DIFFUSE, &mat.diffuse[0]);
		glMaterialfv(GL_FRONT, GL_AMBIENT, &mat.ambient[0]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, &mat.specular[0]);
		glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);

		ogl::Shader shader = ogl::ShaderMgr::instance().get(mat.shader);
		if (shader) {
			shader->bind();
		} else {
			ogl::__Shader::unbind();
		}

	} else {
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);
		glUseProgram(0);
	}
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

Material* MaterialMgr::get(const std::string& name)
{
	std::map<std::string, Material>::iterator itr = m_materials.find(name);
	if (itr == m_materials.end())
		return NULL;
	return &(itr->second);
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
	pair.mat0 = getID(mat0);
	pair.mat1 = getID(mat1);

	if (pair.mat0 > pair.mat1) {
		int tmp = pair.mat0;
		pair.mat0 = pair.mat1;
		pair.mat1 = tmp;
	}

	pair.elasticity = elasticity;
	pair.staticFriction = staticFriction;
	pair.kineticFriction = kineticFriction;
	pair.softness = softness;
	m_pairs[std::make_pair(pair.mat0, pair.mat1)] = pair;

	return std::make_pair(pair.mat0, pair.mat1);
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
				MaterialPair p;
				p.load(node);
				m_pairs[std::make_pair(p.mat0, p.mat1)] = p;
			}
		}
	

		// foreach material tag
			// Material mat("none");
			// mat.load(elem);
			// add(mat);

		// foreach pair tag
			// MaterialPair pair;
			// pair.load(elem);
			// m_pairs[std::make_pair(pair.mat0, pair.mat1)] = pair;

		return true;
	}
	// catch all exceptions
	catch (...) {
		return false;
	}
}

bool MaterialMgr::save(const char* fileName)
{
	using namespace rapidxml;
	// create document
	xml_document<> doc;

	// create XML declaration
	xml_node<>* declaration = doc.allocate_node(node_declaration);
	doc.append_node(declaration);
	declaration->append_attribute(doc.allocate_attribute("version", "1.0"));
    declaration->append_attribute(doc.allocate_attribute("encoding", "utf-8"));


	// create root element "materials"
	xml_node<>* materials = doc.allocate_node(node_element, "materials");
	doc.append_node(materials);
	
	// foreach material
	std::map<std::string, Material>::iterator it;
	for (it = m_materials.begin(); it != m_materials.end(); ++it) {
		it->second.save(materials, &doc);
	}
	
	// foreach pair
	std::map<std::pair<int, int>, MaterialPair>::iterator itp;
	for (itp = m_pairs.begin(); itp != m_pairs.end(); ++itp) {
		itp->second.save(materials, &doc);
	}

	std::string s;
	print(std::back_inserter(s), doc, 0);

	// save document
	std::ofstream myfile;
	myfile.open (fileName);
	myfile << s;
	myfile.close();

	// frees all memory allocated to the nodes
	doc.clear();

	return true;
}


MaterialPair& MaterialMgr::getPair(int mat0, int mat1)
{
	if (mat0 > mat1) {
		int tmp = mat0;
		mat0 = mat1;
		mat1 = tmp;
	}
	// search for interaction
	std::map<std::pair<int, int>, MaterialPair>::iterator it;
	it = m_pairs.find(std::make_pair(mat0, mat1));

	if (it == m_pairs.end())
		// return default pair
		return m_pairs.find(std::make_pair(0, 0))->second;
	else
		// return found pair
		return it->second;
}


void MaterialMgr::processContact(const NewtonJoint* contactJoint, float timestep, int threadIndex)
{
	int mat0, mat1;

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
		mat0 = NewtonMaterialGetBodyCollisionID(material, body0);
		if (mat0 == 0)
			mat0 = NewtonMaterialGetContactFaceAttribute(material);

		// get the second id
		mat1 = NewtonMaterialGetBodyCollisionID(material, body1);
		if (mat1 == 0)
			mat1 = NewtonMaterialGetContactFaceAttribute(material);

		// get the pair for the materials, or the default pair
		MaterialPair& pair = getPair(mat0, mat1);

		//if (mat0 > 0 && mat1 > 0 && (mat0 < 200 || mat1 < 200))
		//	if ((fromID(mat0) && fromID(mat0)->name == "wood_matt") || (fromID(mat1) && fromID(mat1)->name == "wood_matt"))
		//std::cout << "pair " << mat0 << ", " << mat1 << std::endl;

		// set the material properties for this contact
		NewtonMaterialSetContactElasticity(material, pair.elasticity);
		NewtonMaterialSetContactSoftness(material, pair.softness);
		NewtonMaterialSetContactFrictionCoef(material, pair.staticFriction, pair.kineticFriction, 0);
		NewtonMaterialSetContactFrictionCoef(material, pair.staticFriction, pair.kineticFriction, 1);

		/// @todo get impact information and play a sound
	}

	//std::cout << "\tmaterial end" << std::endl;
}

void MaterialMgr::GenericContactCallback(const NewtonJoint* contactJoint,
		dFloat timestep,
		int threadIndex)
{
	instance().processContact(contactJoint, timestep, threadIndex);
}
}
