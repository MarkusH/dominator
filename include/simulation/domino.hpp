/*
 * Domino.hpp
 *
 *  Created on: Jun 7, 2011
 *      Author: markus
 */

#ifndef DOMINO_HPP_
#define DOMINO_HPP_

#include <simulation/object.hpp>



namespace sim {

class __Domino;
typedef std::tr1::shared_ptr<__Domino> Domino;


class __Domino : public __RigidBody {
protected:
	static NewtonCollision* s_domino_collision[3];
	static NewtonCollision* getCollision(Type type, int materialID);
	static Vec3f s_domino_size[3];
public:
	__Domino(Type type, const Mat4f& matrix, const std::string& material = "", int freezeState = 1);
	virtual ~__Domino();

	virtual void genBuffers(ogl::VertexBuffer& vbo);

	static void genDominoBuffers(ogl::VertexBuffer& vbo);
	static void freeCollisions();

	static Domino createDomino(Type type, const Mat4f& matrix, float mass, const std::string& material = "", bool doPlacement = true);
};

}

#endif /* DOMINO_HPP_ */
