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

/**
 * This class tests the m3d module for correctness.
 */
class m3dTest : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(m3dTest);
	CPPUNIT_TEST(invertTest);
	CPPUNIT_TEST(grammSchmidtTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	/**
	 * Tests the m3d matrix multiplication and inversion methods.
	 *
	 * This is done by creating an arbitrary invertible 4x4 matrix and
	 * calculating its inverse. Using matrix multiplication, the product
	 * of these two matrices is calculated and compared to the identity
	 * matrix.
	 */
	void invertTest();

	/**
	 * Tests the m3d Gram Schmidt method.
	 *
	 * This is done by creating two arbitrary 3d vectors: a direction
	 * and a position vector. Using the Gram Schmidt algorithm, an
	 * orthonormal matrix is generated. This matrix is then checked for
	 * sanity (perpendicular axes, correct position, NaN).
	 */
	void grammSchmidtTest();

private:
};

}

#endif /* M3DTEST_HPP_ */
