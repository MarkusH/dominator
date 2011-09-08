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
	CPPUNIT_TEST(saveLoadTest);
	CPPUNIT_TEST(quaternionTest);
	CPPUNIT_TEST(eulerAnglesTest);
	CPPUNIT_TEST(orthonormalInverseTest);
	CPPUNIT_TEST(invertTest);
	CPPUNIT_TEST(gramSchmidtTest);
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Sets a new random seed using the system time.
	 */
	void setUp();
	void tearDown();

protected:

	/**
	 * Tests the vector and matrix save and load methods.
	 *
	 * Creates an arbitrary vector and matrix and saves them to a
	 * string. The resulting string is then used to load
	 * the vector and matrix back. The input and output should be
	 * equal.
	 */
	void saveLoadTest();

	/**
	 * Tests the functionality of the quaternion multiplication and
	 * conversion to a 4x4 matrix.
	 *
	 * This is done by creating an arbitrary two arbitrary axes and
	 * angles. Using these, two matrices and two quaternions describing
	 * the rotation of the angle around the respective axis are created.
	 * The two quaternions are then multiplicated and converted to a
	 * 4x4 rotation matrix. This matrix should be equal to the multiplication
	 * of the two matrices created initially.
	 */
	void quaternionTest();

	/**
	 * Tests the m3d orthonormal inverse and inverse method.
	 *
	 * Creates an arbitrary orthonormal matrix and calculates the inverse
	 * of it using (1) the inverse() and (2) the orthonormalInverse()
	 * method. These should be equal.
	 */
	void orthonormalInverseTest();

	/**
	 * Tests the m3d euler angles extraction method, the rotation matrix
	 * generation and the matrix multiplication.
	 *
	 * Creates an arbitrary rotation matrix and extracts its euler angles.
	 * Using these, three rotation matrices (one for each axis) are created
	 * and multiplied to generate the final rotation matrix. This matrix is
	 * then compared to the initial rotation matrix, which should be equal.
	 */
	void eulerAnglesTest();

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
	void gramSchmidtTest();

private:
};

}

#endif /* M3DTEST_HPP_ */
