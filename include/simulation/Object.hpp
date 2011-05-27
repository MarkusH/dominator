/*
 * Object.hpp
 *
 *  Created on: May 27, 2011
 *      Author: Markus Doellinger
 */

#ifndef OBJECT_HPP_
#define OBJECT_HPP_

#ifdef _WIN32
#include <boost/tr1/memory.hpp>
#else
#include <tr1/memory>
#endif

namespace sim {

class __Object;
typedef std::tr1::shared_ptr<__Object> Object;

class __Object {
public:
	__Object();
	virtual ~__Object();
};

}

#endif /* OBJECT_HPP_ */
