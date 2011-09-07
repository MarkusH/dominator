/**
 * @author Markus Doellinger
 * @date May 28, 2011
 * @file unittests/m3dtest.cpp
 */

#include <unittests/m3dtest.hpp>
#include <m3d/m3d.hpp>

namespace test {

CPPUNIT_TEST_SUITE_REGISTRATION(m3dTest);

void m3dTest::setUp()
{
}

void m3dTest::tearDown()
{
}

static inline float frand(float a = 0.0f, float b = 1.0f)
{
	// return a random float in [a, b]
	return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

void m3dTest::saveLoadTest()
{
	using namespace m3d;
	Vec4f vin(frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f),
			frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f));

	Mat4f min(frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f),
			frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f),
			frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f),
			frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f),
			frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f),
			frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f),
			frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f),
			frand(-10000.0f, 10000.0f), frand(-10000.0f, 10000.0f));

	Vec4f vout;
	Mat4f mout;

	vout.assign(vin.str());
	mout.assign(min.str());

	for (int x = 0; x < 4; ++x) {
		// check if equal, allow a reasonable deviation
		CPPUNIT_ASSERT(fabs(vin[x] - vout[x]) < 0.005f);

		// check for NaN
		CPPUNIT_ASSERT(vout[x] == vout[x]);
	}

	for (int x = 0; x < 4; ++x) {
		for (int y = 0; y < 4; ++y) {
			// check if equal, allow a reasonable deviation
			CPPUNIT_ASSERT(fabs(min[x][y] - mout[x][y]) < 0.005f);

			// check for NaN
			CPPUNIT_ASSERT(mout[x][y] == mout[x][y]);
		}
	}
}

void m3dTest::quaternionTest()
{

}

void m3dTest::orthonormalInverseTest()
{

}

void m3dTest::eulerAnglesTest()
{
	using namespace m3d;
	Vec3f axis(frand(), frand(), frand());
	float angle = frand(0, PI);
	Mat4f input = Mat4f::rotAxis(axis, angle);

	Vec3f euler = input.eulerAngles();

	Mat4f output = Mat4f::rotZ(euler.z) * Mat4f::rotX(euler.x) * Mat4f::rotY(euler.y);

	for (int x = 0; x < 4; ++x) {
		for (int y = 0; y < 4; ++y) {
			// check if equal
			CPPUNIT_ASSERT(fabs(input[x][y] - output[x][y]) < EPSILON);

			// check for NaN
			CPPUNIT_ASSERT(output[x][y] == output[x][y]);
		}
	}
}

void m3dTest::invertTest()
{
	using namespace m3d;
	Vec3f up(0.0f, 1.0f, 0.0f);
	Vec3f dir(frand(), frand(), frand());
	Vec3f pos(frand(), frand(), frand());
	Mat4f matrix(up, dir, pos);
	Mat4f inverse = matrix.inverse();
	Mat4f result = matrix * inverse;
	Mat4f identity = Mat4f::identity();

	for (int x = 0; x < 4; ++x) {
		for (int y = 0; y < 4; ++y) {
			// check if equal
			CPPUNIT_ASSERT(fabs(result[x][y] - identity[x][y]) < EPSILON);

			// check for NaN
			CPPUNIT_ASSERT(result[x][y] == result[x][y]);
		}
	}
}

void m3dTest::gramSchmidtTest()
{
	using namespace m3d;
	Vec3f dir(frand(), frand(), frand());
	Vec3f pos(frand(), frand(), frand());
	Mat4f matrix = Mat4f::gramSchmidt(dir, pos);

	// check if all axes are perpendicular
	CPPUNIT_ASSERT(fabs(matrix.getX() * matrix.getY()) < EPSILON);
	CPPUNIT_ASSERT(fabs(matrix.getY() * matrix.getZ()) < EPSILON);
	CPPUNIT_ASSERT(fabs(matrix.getX() * matrix.getZ()) < EPSILON);

	for (int x = 0; x < 4; ++x) {
		for (int y = 0; y < 4; ++y) {

			// check for NaN
			CPPUNIT_ASSERT(matrix[x][y] == matrix[x][y]);
		}
	}

	// check if position is correct
	CPPUNIT_ASSERT(matrix.getW() == pos);
}


}
