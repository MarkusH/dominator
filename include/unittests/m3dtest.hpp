/**
 * @author Markus Doellinger
 * @date May 28, 2011
 * @file unittests/m3dtest.hpp
 */

#ifndef M3DTEST_HPP_
#define M3DTEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace test {

class m3dTest : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(m3dTest);
	CPPUNIT_TEST(invertTest);
	CPPUNIT_TEST(grammSchmidtTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void invertTest();
	void grammSchmidtTest();

private:
};

}

#endif /* M3DTEST_HPP_ */
