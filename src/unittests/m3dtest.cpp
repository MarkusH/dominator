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
	return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

void m3dTest::invertTest()
{
	using namespace m3d;
	Mat4f matrix(frand(), frand(), frand(), frand(),
				 frand(), frand(), frand(), frand(),
				 frand(), frand(), frand(), frand(),
				 frand(), frand(), frand(), frand());
	Mat4f inverse = matrix.inverse();
	Mat4f result = matrix * inverse;
	Mat4f identity = Mat4f::identity();

	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			CPPUNIT_ASSERT(fabs(result[x][y] - identity[x][y]) < EPSILON);
}

void m3dTest::grammSchmidtTest()
{
	CPPUNIT_ASSERT(true);
}


}
