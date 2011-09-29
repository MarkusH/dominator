/**
 * @author Robert Waury
 * @date September 22, 2011
 * @file unittests/xmltest.cpp
 */

#include <unittests/xmltest.hpp>
#include <util/config.hpp>
#include <util/inputadapters.hpp>
#include <simulation/simulation.hpp>
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
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(no_such_file));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/levels/bridge.xml";
		CPPUNIT_ASSERT(!sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}

	void xmlTest::loadLevelEmptyRootTest() {
		util::MouseAdapter ma;
		util::KeyAdapter ka;
		sim::Simulation::createInstance(ka, ma);
		std::string filename = "data/unittest_xml/level/level_empty_root.xml";
		CPPUNIT_ASSERT(sim::Simulation::instance().load(filename));
		sim::Simulation::destroyInstance();
	}
	/* level file tests END */

}