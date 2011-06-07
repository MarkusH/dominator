/*
 * Domino.hpp
 *
 *  Created on: Jun 7, 2011
 *      Author: markus
 */

#ifndef DOMINO_HPP_
#define DOMINO_HPP_

#include <simulation/Object.hpp>

namespace sim {

class __Domino;
typedef std::tr1::shared_ptr<__Domino> Domino;


class __Domino : public __RigidBody {
public:
	__Domino(Type type, const Mat4f& matrix, const std::string& material = "");
	virtual ~__Domino();

	static Domino createDomino(const Type& type, const Mat4f& matrix, float mass, const std::string& material = "");
};

}

#endif /* DOMINO_HPP_ */
