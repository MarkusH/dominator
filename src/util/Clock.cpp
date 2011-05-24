/*
 * Clock.cpp
 *
 *  Created on: May 23, 2011
 *      Author: markus
 */

#include <util/Clock.hpp>
#ifdef _WIN32
	// TODO useful include
#else
#include <sys/time.h>
#endif

namespace util
{

Clock::Clock()
{
	reset();
}

Clock::~Clock()
{
}

double Clock::sysTime() const
{
#ifdef _WIN32
	// TODO: use query performance counter
#else
	timeval time = { 0, 0 };
	gettimeofday(&time, 0);
	return time.tv_sec + time.tv_usec / 1000000.0;
#endif
}

void Clock::reset()
{
	m_reference = sysTime();
}

float Clock::get() const
{
	return sysTime() - m_reference;
}

}
