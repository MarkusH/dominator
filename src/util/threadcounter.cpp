/**
 * @author Robert Waury
 * @date Jun 30, 2011
 * @file util/threadcounter.cpp
 */

#include <util/threadcounter.hpp>
#include <boost/thread/thread.hpp>

namespace util {

int getThreadCount() {

	unsigned threadcount = boost::thread::hardware_concurrency();

	if(!threadcount) return 1;
	if(threadcount > 8) return 8;
	else return (int)threadcount;
}

}
