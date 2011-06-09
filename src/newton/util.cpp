/*
 * util.cpp
 *
 *  Created on: May 28, 2011
 *      Author: Markus Doellinger
 */

#include <opengl/oglutil.hpp>
#include <newton/util.hpp>
#include <iostream>
#include <dVector.h>
#include <dMatrix.h>


namespace newton {

struct RayCastBodyData {
	NewtonBody* body;
	float param;
};

static float getRayCastBodyCallback(const NewtonBody* body, const float* normal, int collisionID, void* userData, float intersectParam)
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


static float getVerticalPositionCallback(const NewtonBody* body, const float* normal, int collisionID, void* userData, float intersectParam)
{
	float* paramPtr = (float*)userData;
	if (intersectParam < paramPtr[0]) {
		paramPtr[0] = intersectParam;
	}
	return paramPtr[0];
}

float getVerticalPosition(const NewtonWorld* world, float x, float z)
{
	float parameter;

	// shoot a vertical ray from a high altitude and collect the intersection parameter.
	Vec3f p0(x, 1000.0f, z);
	Vec3f p1(x, -1000.0f, z);

	parameter = 1.2f;
	NewtonWorldRayCast(world, &p0[0], &p1[0], getVerticalPositionCallback, &parameter, NULL);
	//_ASSERTE (parameter < 1.0f);

	// the intersection is the interpolated value
	return 1000.0f - 2000.0f * parameter;
}


static unsigned ConvexCastCallback(const NewtonBody* body, const NewtonCollision* collision, void* userData)
{
	//std::cout << body << std::endl;
	NewtonBody* other = (NewtonBody*)userData;
	return (other == body) ? 0 : 1;
}


float getConvexCastPlacement(NewtonBody* body)
{
	//std::cout << "convex cast for_ " << body << std::endl;
	Mat4f matrix;
	NewtonBodyGetMatrix(body, matrix[0]);
	matrix._42 += 200.0f;
	Vec3f p(matrix.getW());
	p.y -= 400.0f;

	NewtonWorld* world = NewtonBodyGetWorld(body);
	NewtonCollision* collision = NewtonBodyGetCollision(body);

	float param = 0.0f;
	NewtonWorldConvexCastReturnInfo info[16];

	NewtonCollisionInfoRecord collisionInfo;
	NewtonCollisionGetInfo(collision, &collisionInfo);

	if (collisionInfo.m_collisionType == SERIALIZE_ID_COMPOUND) {
		float maximum = -200.0f;
		for (int i = 0; i < collisionInfo.m_compoundCollision.m_chidrenCount; ++i) {
			NewtonCollision* node = collisionInfo.m_compoundCollision.m_chidren[i];
			NewtonWorldConvexCast(world, matrix[0], &p[0], node, &param, body, ConvexCastCallback, info, 16, 0);
			float current = matrix._42 + (p.y - matrix._42) * param;
			if (current > maximum) maximum = current;
		}
		return maximum;
	} else {
		NewtonWorldConvexCast(world, matrix[0], &p[0], collision, &param, body, ConvexCastCallback, info, 16, 0);

		matrix._42 += (p.y - matrix._42) * param;
		//std::cout << "placement is " << matrix._42 << std::endl;
		return matrix._42;
	}
}



static void debugShowGeometryCollision(void* userData, int vertexCount, const float* faceVertec, int id)
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


#define MOUSE_PICK_DAMP			 10.0f
#define MOUSE_PICK_STIFFNESS	 80.0f
//#define MOUSE_PICK_STIFFNESS	 50.0f

static float pickedParam;
static dVector pickedForce;
static dVector rayLocalNormal;
static dVector rayWorldNormal;
static dVector attachmentPoint;

static bool down0;
static Vec2f mouse0;
static bool mousePickMode = false;

dVector p0;
dVector p1;

static bool isPickedBodyDynamics;
static NewtonBody* pickedBody;
static NewtonApplyForceAndTorque chainForceCallBack;


static float mousePickRayCastFilter(const NewtonBody* body, const float* normal, int collisionID, void* userData, float intersectParam)
{
	float mass, Ixx, Iyy, Izz;

	NewtonBodyGetMassMatrix(body, &mass, &Ixx, &Iyy, &Izz);
	if (intersectParam < pickedParam) {
		isPickedBodyDynamics = (mass > 0.0f);
		pickedParam = intersectParam;
		pickedBody = (NewtonBody*)body;
		rayLocalNormal = dVector(normal[0], normal[1], normal[2]);
	}
	return intersectParam;
}



static void PhysicsApplyPickForce (const NewtonBody* body, float timestep, int threadIndex)
{
	float mass, Ixx, Iyy, Izz;
	dVector com, veloc, omega;
	dMatrix matrix;

	{
		dMatrix matrix;
		NewtonBodyGetMatrix(pickedBody, &matrix[0][0]);

		dVector p2 (matrix.TransformVector(attachmentPoint));
		dVector p (p0 + (p1 - p0).Scale (pickedParam));
		pickedForce = p - p2;

		float mag2 = pickedForce % pickedForce;
		if (mag2 > float (20 * 20)) {
			pickedForce = pickedForce.Scale (20.0f / dSqrt (pickedForce % pickedForce));
		}

		// rotate normal to global space
		rayWorldNormal = matrix.RotateVector(rayLocalNormal);
	}

	// apply the default body forces
	if (chainForceCallBack) {
		chainForceCallBack (body, timestep, threadIndex);
	}

	// add the mouse pick penalty force and torque
	NewtonBodyGetVelocity(body, &veloc[0]);

	NewtonBodyGetOmega(body, &omega[0]);
	NewtonBodyGetVelocity(body, &veloc[0]);
	NewtonBodyGetMassMatrix(body, &mass, &Ixx, &Iyy, &Izz);

	dVector force (pickedForce.Scale (mass * MOUSE_PICK_STIFFNESS));
	dVector dampForce(veloc.Scale (mass * MOUSE_PICK_DAMP));
	force -= dampForce;

	NewtonBodyGetMatrix(body, &matrix[0][0]);
	NewtonBodyGetCentreOfMass (body, &com[0]);

	// calculate local point relative to center of mass
	dVector point (matrix.RotateVector(attachmentPoint - com));
	dVector torque (point * force);

	dVector torqueDamp (omega.Scale (mass * 0.1f));

	NewtonBodyAddForce (body, &force.m_x);
	NewtonBodyAddTorque (body, &torque.m_x);

	// make sure the body is unfrozen, if it is picked
	NewtonBodySetFreezeState (body, 0);
}


bool mousePick(const NewtonWorld* world, const Vec2f& mouse, bool down)
{
	dMatrix matrix;

	if (down) {
		if (!down0) {
			Vec3f _p0(ogl::screenToWorld(Vec3d(mouse.x, mouse.y, 0.0f)));
			Vec3f _p1(ogl::screenToWorld(Vec3d(mouse.x, mouse.y, 1.0f)));
			p0 = dVector(_p0.x, _p0.y, _p0.z, 0.0f);
			p1 = dVector(_p1.x, _p1.y, _p1.z, 0.0f);

			pickedBody = NULL;
			pickedParam = 1.1f;
			isPickedBodyDynamics = false;
			NewtonWorldRayCast(world, &p0[0], &p1[0], mousePickRayCastFilter, NULL, NULL);
			if (pickedBody) {
				mousePickMode = true;
				//NewtonBodySetFreezeState (pickedBody, 0);
				NewtonBodyGetMatrix(pickedBody, &matrix[0][0]);
				dVector p (p0 + (p1 - p0).Scale (pickedParam));

				// save point local to th body matrix
				attachmentPoint = matrix.UntransformVector (p);

				// convert normal to local space
				rayLocalNormal = matrix.UnrotateVector(rayLocalNormal);

				// save the transform call back
				chainForceCallBack = NewtonBodyGetForceAndTorqueCallback (pickedBody);
				_ASSERTE (chainForceCallBack != PhysicsApplyPickForce);

				// set a new call back
				NewtonBodySetForceAndTorqueCallback (pickedBody, PhysicsApplyPickForce);
			}
		}

		if (mousePickMode) {
			// init pick mode
			dMatrix matrix;
			NewtonBodyGetMatrix(pickedBody, &matrix[0][0]);
			Vec3f _p0 (ogl::screenToWorld(Vec3d(mouse.x, mouse.y, 0.0f)));
			Vec3f _p1 (ogl::screenToWorld(Vec3d(mouse.x, mouse.y, 1.0f)));
			p0 = dVector(_p0.x, _p0.y, _p0.z, 0.0f);
			p1 = dVector(_p1.x, _p1.y, _p1.z, 0.0f);
			dVector p2 (matrix.TransformVector (attachmentPoint));

			dVector p (p0 + (p1 - p0).Scale (pickedParam));
			pickedForce = p - p2;
			float mag2 = pickedForce % pickedForce;
			if (mag2 > float (20 * 20)) {
				pickedForce = pickedForce.Scale (20.0f / dSqrt (pickedForce % pickedForce));
			}

			// rotate normal to global space
			rayWorldNormal = matrix.RotateVector(rayLocalNormal);

			//TODO: show the pick points
		}
	} else {
		mousePickMode = false;
		if (pickedBody) {
			NewtonBodySetForceAndTorqueCallback (pickedBody, chainForceCallBack);
			pickedBody = NULL;
			chainForceCallBack = NULL;
		}
	}

	mouse0 = mouse;
	down0 = down;

	bool retState;
	retState = isPickedBodyDynamics;
	return retState;
}


}
