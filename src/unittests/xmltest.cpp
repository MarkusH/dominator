/**
 * @author Robert Waury
 * @date September 22, 2011
 * @file unittests/xmltest.cpp
 */

#include <unittests/xmltest.hpp>
#include <util/config.hpp>
#include <util/inputadapters.hpp>
#include <simulation/simulation.hpp>
#include <simulation/material.hpp>
#include <string>

namespace test {

	CPPUNIT_TEST_SUITE_REGISTRATION(xmlTest);

	void xmlTest::setUp(){
	}
	void xmlTest::tearDown(){
		
	}

	/* config file tests */
	void xmlTest::noConfigFileTest() {
		std::string no_such_file = "data/no_such_file.xml";
		CPPUNIT_ASSERT(!util::Config::instance().load(no_such_file));
	}

	void xmlTest::loadConfigTest() {
		std::string filename = "data/config.xml";
		CPPUNIT_ASSERT(util::Config::instance().load(filename));
	}

	void xmlTest::loadConfigKeyMissingTest() {
		std::string filename = "data/unittest_xml/config/config_key_missing.xml";
		CPPUNIT_ASSERT(!util::Config::instance().load(filename));
	}

	void xmlTest::loadConfigValueMissingTest() {
		std::string filename = "data/unittest_xml/config/config_value_missing.xml";
		CPPUNIT_ASSERT(!util::Config::instance().load(filename));
	}

	void xmlTest::loadConfigNoRootTest() {
		std::string filename = "data/unittest_xml/config/config_no_root.xml";
		CPPUNIT_ASSERT(!util::Config::instance().load(filename));
	}

	void xmlTest::loadConfigEmptyRootTest() {
		std::string filename = "data/unittest_xml/config/config_empty_root.xml";
		CPPUNIT_ASSERT(util::Config::instance().load(filename));
	}

	void xmlTest::loadConfigUnknownTagTest() {
		std::string filename = "data/unittest_xml/config/config_unknown_tag.xml";
		CPPUNIT_ASSERT(util::Config::instance().load(filename));
	}

	void xmlTest::loadConfigBrokenTest() {
		std::string filename = "data/unittest_xml/config/config_broken.xml";
		CPPUNIT_ASSERT(!util::Config::instance().load(filename));
	}
	/* config file tests END */

	/* level file tests */
	void xmlTest::noLevelFileTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string no_such_file = "data/levels/no_such_file.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(no_such_file));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/levels/bridge.xml";
		// should work
		CPPUNIT_ASSERT(sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelEmptyRootTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_empty_root.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelEnvOnlyTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_environment_only.xml";
		// should work
		CPPUNIT_ASSERT(sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelNoEnvTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_no_environment.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelNoRootTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_no_root.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelCompoundMatrixBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_compound_matrix_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelRootEyeBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_eye_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelRootPositionBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_position_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelRootUpBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_up_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelJointTypeBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_joint_type_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelJointPivotBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_pivot_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelJointPinDirBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_pindir_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelEnvNoSuchModelTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_environment_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelObjectTypeBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_object_type_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelObjectMaterialBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_material_broken.xml";
		// expected to work
		CPPUNIT_ASSERT(sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelObjectMatrixBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_object_matrix_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelObjectDampingBrokenTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_damping_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}
	/* level file tests END */

	/* materials file test */
	void xmlTest::noMaterialsFileTest() {
		std::string no_such_file = "data/no_such_file.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::MaterialMgr::instance().load(no_such_file));	
	}

	void xmlTest::loadMaterialsTest() {
		std::string filename = "data/materials.xml";
		// expected to work
		CPPUNIT_ASSERT(sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsAmbientBrokenTest() {
		std::string filename = "data/unittest_xml/material/materials_ambient_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsDiffuseBrokenTest() {
		std::string filename = "data/unittest_xml/material/materials_diffuse_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsSpecularBrokenTest() {
		std::string filename = "data/unittest_xml/material/materials_specular_broken.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsEmptyRoot() {
		std::string filename = "data/unittest_xml/material/materials_empty_root.xml";
		// expected to work
		CPPUNIT_ASSERT(sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsNoRoot() {
		std::string filename = "data/unittest_xml/material/materials_no_root.xml";
		// expected to fail
		CPPUNIT_ASSERT(!sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsNoPairsTest() {
		std::string filename = "data/unittest_xml/material/materials_no_pairs.xml";
		// expected to work (everything is the default pair)
		CPPUNIT_ASSERT(sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsNoMaterialsTest() {
		std::string filename = "data/unittest_xml/material/materials_no_materials.xml";
		// expected to work (everything is the default pair)
		CPPUNIT_ASSERT(sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsNoSuchShaderTest() {
		std::string filename = "data/unittest_xml/material/materials_no_such_shader.xml";
		// expected to work because the ShaderMgr always returns something
		CPPUNIT_ASSERT(sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsNoSuchSoundTest() {
		std::string filename = "data/unittest_xml/material/materials_no_such_sound.xml";
		// expected to work because the SoundMgr just doesn't play the sound
		CPPUNIT_ASSERT(sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsNoSuchTextureTest() {
		std::string filename = "data/unittest_xml/material/materials_no_such_texture.xml";
		// expected to work because the TextureMgr always returns something
		CPPUNIT_ASSERT(sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsNoSuchTexture1Test() {
		std::string filename = "data/unittest_xml/material/materials_no_such_texture1.xml";
		// expected to work (same reason as above)
		CPPUNIT_ASSERT(sim::MaterialMgr::instance().load(filename));
	}

	void xmlTest::loadMaterialsUnknownMatInPairTest() {
		std::string filename = "data/unittest_xml/material/materials_unknown_material_in_pair.xml";
		// expected to work since the default pair is used as a fallback
		CPPUNIT_ASSERT(sim::MaterialMgr::instance().load(filename));
	}

	/* materials file test END */

}