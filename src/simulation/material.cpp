/**
 * @author Markus Doellinger, Robert Waury
 * @date May 24, 2011
 * @file simulation/material.cpp
 */

#include <simulation/material.hpp>
#include <opengl/texture.hpp>
#include <opengl/shader.hpp>
#include <GL/glew.h>
#include <limits.h>
#include <boost/functional/hash.hpp>
#include <Newton.h>
#include <xml/rapidxml_utils.hpp>
#include <xml/rapidxml_print.hpp>
#include <fstream>
#include <string.h>
#include <util/tostring.hpp>
#include <util/erroradapters.hpp>
#include <clocale>
#include <sound/soundmgr.hpp>
#include <simulation/simulation.hpp>

namespace sim {

using namespace m3d;

MaterialMgr* MaterialMgr::s_instance = NULL;

Material::Material(const std::string& name)
		: name(name)
{
}

Material::Material(const Material& m)
	: name(m.name),
	  texture(m.texture),
	  texture1(m.texture1),
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

	xml_attribute<>* attr = node->first_attribute("name");
	if(attr) {
	name = attr->value();
	} else throw parse_error("No \"name\" attribute in material tag found", node->name());


	attr = node->first_attribute("texture");
	if(attr) {
	texture = attr->value();
	} else throw parse_error("No \"texture\" attribute in material tag found", node->name());

	// texture1 is optional
	attr = node->first_attribute("texture1");
	texture1 = attr ? attr->value() : "";

	attr = node->first_attribute("shader");
	if(attr) {
	shader = attr->value();
	} else throw parse_error("No \"shader\" attribute in material tag found", node->name());


	attr = node->first_attribute("ambient");
	if(attr) {
	ambient.assign(attr->value());
	} else throw parse_error("No \"ambient\" attribute in material tag found", node->name());


	attr = node->first_attribute("diffuse");
	if(attr) {
	diffuse.assign(attr->value());
	} else throw parse_error("No \"diffuse\" attribute in material tag found", node->name());


	attr = node->first_attribute("specular");
	if(attr) {
	specular.assign(attr->value());
	} else throw parse_error("No \"specular\" attribute in material tag found", node->name());

	
	attr = node->first_attribute("shininess");
	if(attr) {
	shininess = atof(attr->value());
	} else throw parse_error("No \"shininess\" attribute in material tag found", node->name());


}


void Material::save(rapidxml::xml_node<>* materials, rapidxml::xml_document<>* doc) const
{
	using namespace rapidxml;

	// create material node
	xml_node<>* node = doc->allocate_node(node_element, "material");
	materials->append_node(node);

	/* allocate strings for attributes  and append them to node */

	char* pName = doc->allocate_string(name.c_str());
	xml_attribute<>* attrN = doc->allocate_attribute("name", pName);
	node->append_attribute(attrN);

	char* pTexture = doc->allocate_string(texture.c_str());
	xml_attribute<>* attrT = doc->allocate_attribute("texture", pTexture);
	node->append_attribute(attrT);

	if (texture1.size() > 0) {
		char* pTexture1 = doc->allocate_string(texture1.c_str());
		xml_attribute<>* attrT = doc->allocate_attribute("texture1", pTexture1);
		node->append_attribute(attrT);
	}

	char* pShader = doc->allocate_string(shader.c_str());
	xml_attribute<>* attrS = doc->allocate_attribute("shader", pShader);
	node->append_attribute(attrS);

	char* pAmbient = doc->allocate_string(ambient.str().c_str());
	xml_attribute<>* attrA = doc->allocate_attribute("ambient", pAmbient);
	node->append_attribute(attrA);

	char* pDiffuse = doc->allocate_string(diffuse.str().c_str());
	xml_attribute<>* attrD = doc->allocate_attribute("diffuse", pDiffuse);
	node->append_attribute(attrD);

	char* pSpecular = doc->allocate_string(specular.str().c_str());
	xml_attribute<>* attrSp = doc->allocate_attribute("specular", pSpecular);
	node->append_attribute(attrSp);

	//char* pShininess = doc->allocate_string(boost::lexical_cast<std::string>(shininess).c_str());
	char* pShininess = doc->allocate_string(util::toString(shininess));
	xml_attribute<>* attrSh = doc->allocate_attribute("shininess", pShininess);
	node->append_attribute(attrSh);
	//free(pShininess);

	/* END allocate strings for attributes  and append them to node */

}


MaterialPair::MaterialPair()
{
	mat0 = mat1 = 0;
	elasticity = 0.4f;
	staticFriction = 0.9f;
	kineticFriction = 0.5f;
	softness = 0.1f;
	impactSound = "";
}

MaterialPair::MaterialPair(const MaterialPair& p)
	: mat0(p.mat0),
	  mat1(p.mat1),
	  elasticity(p.elasticity),
	  staticFriction(p.staticFriction),
	  kineticFriction(p.kineticFriction),
	  softness(p.softness),
	  impactSound(p.impactSound)
{
}


void MaterialPair::load(rapidxml::xml_node<>* node)
{
	setlocale(LC_ALL, "C");
	using namespace rapidxml;

	MaterialMgr& m = MaterialMgr::instance();
	
	xml_attribute<> *attr = node->first_attribute("mat0");
	if(attr) {
	mat0 = m.getID(attr->value());
	} else throw parse_error("No \"mat0\" attribute in pair tag found", node->name());


	attr = node->first_attribute("mat1");
	if(attr) {
	mat1 = m.getID(attr->value());
	} else throw parse_error("No \"mat1\" attribute in pair tag found", node->name());


	attr = node->first_attribute("elasticity");
	if(attr) {
	elasticity = atof(attr->value());
	} else throw parse_error("No \"elasticity\" attribute in pair tag found", node->name());


	attr = node->first_attribute("staticFriction");
	if(attr) {
	staticFriction = atof(attr->value());
	} else throw parse_error("No \"staticFriction\" attribute in pair tag found", node->name());


	attr = node->first_attribute("kineticFriction");
	if(attr) {
	kineticFriction = atof(attr->value());
	} else throw parse_error("No \"kineticFriction\" attribute in pair tag found", node->name());


	attr = node->first_attribute("softness");
	if(attr) {
	softness = atof(attr->value());
	} else throw parse_error("No \"softness\" attribute in pair tag found", node->name());

	attr = node->first_attribute("impactSound");
	if(attr) {
		impactSound = attr->value();
	} else impactSound = MaterialMgr::instance().getPair(0, 0).impactSound;


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

	/* allocate strings for attributes  and append them to node */
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

	//char* pElasticity = doc->allocate_string(boost::lexical_cast<std::string>(elasticity).c_str());
	char* pElasticity = doc->allocate_string(util::toString(elasticity));
	xml_attribute<>* attrE = doc->allocate_attribute("elasticity", pElasticity);
	node->append_attribute(attrE);
	//free(pElasticity);

	//char* pStaticFriction = doc->allocate_string(boost::lexical_cast<std::string>(staticFriction).c_str());
	char* pStaticFriction = doc->allocate_string(util::toString(staticFriction));
	xml_attribute<>* attrSF = doc->allocate_attribute("staticFriction", pStaticFriction);
	node->append_attribute(attrSF);
	//free(pStaticFriction);

	//char* pKineticFriction = doc->allocate_string(boost::lexical_cast<std::string>(kineticFriction).c_str());
	char* pKineticFriction = doc->allocate_string(util::toString(kineticFriction));
	xml_attribute<>* attrKF = doc->allocate_attribute("kineticFriction", pKineticFriction);
	node->append_attribute(attrKF);
	//free(pKineticFriction);

	//char* pSoftness = doc->allocate_string(boost::lexical_cast<std::string>(softness).c_str());
	char* pSoftness = doc->allocate_string(util::toString(softness));
	xml_attribute<>* attrS = doc->allocate_attribute("softness", pSoftness);
	node->append_attribute(attrS);
	//free(pSoftness);

	const char* pImpactSound = doc->allocate_string(impactSound.c_str());
	xml_attribute<>* attrIS = doc->allocate_attribute("impactSound", pImpactSound);
	node->append_attribute(attrIS);
	/* END allocate strings for attributes  and append them to node */

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
	s_instance = NULL;
}

int MaterialMgr::getMaterials(std::set<std::string>& materials)
{
	std::map<std::string, Material>::const_iterator itr = m_materials.begin();
	for ( ; itr != m_materials.end(); ++itr)
		materials.insert(itr->first);
	return m_materials.size();
}

void MaterialMgr::applyMaterial(const std::string& material, bool useShadows) {
	const Material* const _mat = get(material);
	/// @todo only switch shader/texture if necessary
	if (_mat != NULL) {
		const Material& mat = *_mat;
		ogl::Texture texture = ogl::TextureMgr::instance().get(mat.texture);
		ogl::Texture texture1 = ogl::TextureMgr::instance().get(mat.texture1);

		if (texture) {
			glEnable(GL_TEXTURE_2D);
			texture->bind();
		} else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		if (texture1) {
			glEnable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE1);
			texture1->bind();
			glActiveTexture(GL_TEXTURE0);
		} else {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
			glActiveTexture(GL_TEXTURE0);
		}

		GLfloat emission[4] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, &mat.diffuse[0]);
		glMaterialfv(GL_FRONT, GL_AMBIENT, &mat.ambient[0]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, &mat.specular[0]);
		glMaterialfv(GL_FRONT, GL_EMISSION, &emission[0]);
		glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);

		ogl::Shader shader = ogl::ShaderMgr::instance().get(mat.shader);
		if (shader) {
			shader->bind();
			shader->setUniform1i("Texture0", 0);
			shader->setUniform1i("Texture1", 1);

			if (useShadows) {
				shader->setUniform1i("ShadowMap", 7);
				shader->setUniform1f("shadowTexel", 1.0 / SHADOW_MAP_SIZE);
			}
		} else {
			ogl::__Shader::unbind();
		}

	} else {
		//glDisable(GL_TEXTURE_2D);
		//glColor3f(1.0f, 1.0f, 1.0f);
		//glUseProgram(0);
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

	boost::hash<std::string> string_hash;
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
		boost::hash<std::string> string_hash;
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

bool MaterialMgr::load(const std::string& fileName)
{
	/* information for error messages */
	std::string function = "MaterialMgr::load";
	std::vector<std::string> args;
	args.push_back(fileName);
	/* END information for error messages */

	using namespace rapidxml;

	// this prevents that the atof functions fails on German systems
	// since they use "," as a separator for floats
	setlocale(LC_ALL,"C");

	file<char>* f = 0;

	try {
		f = new file<char>(fileName.c_str());
	} catch ( std::runtime_error& e ) {
		util::ErrorAdapter::instance().displayErrorMessage(function, args, e);
		if(f) delete f;
		return false;
	} catch (...) {
		util::ErrorAdapter::instance().displayErrorMessage(function, args);
		if(f) delete f;
		return false;
	}

	try {
		xml_document<> materials;
		materials.parse<0>(f->data());

		// this is important so we don't parse the materials tag but the material and pair tags
		xml_node<>* nodes = materials.first_node("materials");
		if( nodes ) { 
			for (xml_node<>* node = nodes->first_node("material"); node; node = node->next_sibling("material")) {
				Material mat("none");
				mat.load(node);
				add(mat);
			}
			for (xml_node<>* node = nodes->first_node("pair"); node; node = node->next_sibling("pair")) {
				MaterialPair p;
				p.load(node);
				m_pairs[std::make_pair(p.mat0, p.mat1)] = p;
			}
			delete f;
			return true;
		} else {
			delete f;
			//throw parse_error("No valid root node found", m);
			return false;
		}
	} catch( parse_error& e ) {
		util::ErrorAdapter::instance().displayErrorMessage(function, args, e);
		delete f;
		return false;
	} catch(...) {
		util::ErrorAdapter::instance().displayErrorMessage(function, args);
		delete f;
		return false;
	}
}

bool MaterialMgr::save(const std::string& fileName)
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
	myfile.open (fileName.c_str());
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
	Vec3f contactPos, contactNormal;
	float bestNormalSpeed = 6.7f;
	std::string bestSound = "";

	int mat0, mat1;

	// get the contact material
	NewtonMaterial* material;
	NewtonBody *body0, *body1;

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

		//std::cout << "pair " << mat0 << ", " << mat1 << std::endl;
		//std::cout <<pair.elasticity << std::endl;

		// set the material properties for this contact
		NewtonMaterialSetContactElasticity(material, pair.elasticity);
		NewtonMaterialSetContactSoftness(material, pair.softness);
		NewtonMaterialSetContactFrictionCoef(material, pair.staticFriction, pair.kineticFriction, 0);
		NewtonMaterialSetContactFrictionCoef(material, pair.staticFriction, pair.kineticFriction, 1);

		float normalSpeed = NewtonMaterialGetContactNormalSpeed(material);
		if (normalSpeed > bestNormalSpeed){
			bestNormalSpeed = normalSpeed;
			NewtonMaterialGetContactPositionAndNormal(material, body0, &contactPos[0], &contactNormal[0]);
			bestSound = pair.impactSound;
		}

		/*
		bool conv = false;
		Material* mat = fromID(mat0);
		if (mat && mat->name == "sand")
			conv = true;

		mat = fromID(mat1);
		if (mat && mat->name == "sand")
			conv = true;

		if (conv) {
			//std::cout << "conv" << std::endl;
			Vec3f convDir(1.0f, 0.0f, 0.0f);
			float convSpeed = 10.0f;
			NewtonBodySetFreezeState(body0, 0); NewtonBodySetFreezeState(body1, 0);
			NewtonMaterialContactRotateTangentDirections(material, &convDir[0]);
			float speed = NewtonMaterialGetContactTangentSpeed(material, 0);
			NewtonMaterialSetContactElasticity(material, 0);
		    NewtonMaterialSetContactTangentAcceleration(material, (convSpeed - speed) / timestep, 0);
		}
	*/
	}

	if (bestSound.size()) {
		Vec3f distance(Simulation::instance().getCamera().m_position - contactPos);
		float dist2 = distance * distance;
		if (dist2 < (MAX_SOUND_DISTANCE * MAX_SOUND_DISTANCE)) {
			Vec3f vel;
			snd::SoundMgr::instance().PlaySound(bestSound, 1, &contactPos[0], &vel[0]);
		}
	}
}

void MaterialMgr::GenericContactCallback(const NewtonJoint* contactJoint,
		dFloat timestep,
		int threadIndex)
{
	instance().processContact(contactJoint, timestep, threadIndex);
}
}
