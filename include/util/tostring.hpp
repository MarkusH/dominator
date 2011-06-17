#ifndef TOSTRING_HPP_
#define TOSTRING_HPP_

#include <iostream>
#include <string>

namespace util {

template<typename T>
char* toString(T value)
{
	std::stringstream sst;
	sst << value;
	sst.seekg(0, std::ios::beg);
	return strdup(sst.str().c_str());
}

}

#endif /* TOSTRING_HPP_ */
