/*
 * util.cpp
 *
 *  Created on: May 28, 2011
 *      Author: Markus Doellinger
 */

#include <newton/util.hpp>

namespace newton {

struct RayCastBodyData {
	NewtonBody* body;
	dFloat param;
};

static dFloat getRayCastBodyCallback(const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersectParam)
{
	RayCastBodyData* data = (RayCastBodyData*)userData;
	if (intersectParam < data->param) {
		data->param = intersectParam;
		data->body = const_cast<NewtonBody*>(body);
	}
	return data->param;
}

NewtonBody* getRayCastBody(const NewtonWorld* world, const Vec3f& origin, const Vec3f& dir)
{
	RayCastBodyData data;
	data.param = 1.2f;
	data.body = NULL;
	Vec3f dest = origin + dir.normalized() * 1000.0f;
	NewtonWorldRayCast(world, &origin[0], &dest[0], getRayCastBodyCallback, &data, NULL);
	return data.body;
}


static dFloat getVerticalPositionCallback(const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersectParam)
{
	dFloat* paramPtr = (dFloat*)userData;
	if (intersectParam < paramPtr[0]) {
		paramPtr[0] = intersectParam;
	}
	return paramPtr[0];
}

float getVerticalPosition(const NewtonWorld* world, float x, float z)
{
	dFloat parameter;
	// shot a vertical ray from a high altitude and collect the intersection parameter.
	Vec3f p0(x, 1000.0f, z);
	Vec3f p1(x, -1000.0f, z);

	parameter = 1.2f;
	NewtonWorldRayCast(world, &p0[0], &p1[0], getVerticalPositionCallback, &parameter, NULL);
	//_ASSERTE (parameter < 1.0f);

	// the intersection is the interpolated value
	return 1000.0f - 2000.0f * parameter;
}


static void debugShowGeometryCollision(void* userData, int vertexCount, const dFloat* faceVertec, int id)
{
	int i;

	i = vertexCount - 1;
	Vec3f p0 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
	for (i = 0; i < vertexCount; i ++) {
		Vec3f p1 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
		glVertex3f(p0.x, p0.y, p0.z);
		glVertex3f(p1.x, p1.y, p1.z);
		p0 = p1;
	}
}

void showCollisionShape(const NewtonCollision* shape, const Mat4f& matrix)
{
	glBegin(GL_LINES);
	NewtonCollisionForEachPolygonDo(shape, matrix[0], debugShowGeometryCollision, NULL);
	glEnd();
}


}
