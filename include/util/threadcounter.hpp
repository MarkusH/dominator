#ifndef THREADCOUNTER_HPP_
#define THREADCOUNTER_HPP_

#include <boost/thread/thread.hpp>

int getThreadCount() {

	unsigned threadcount = boost::thread::hardware_concurrency();

	if(threadcount) return 2;
	if(threadcount > 8) return 8;
	else return (int)threadcount;
}

#endif /* END THREADCOUNTER_HPP_ */