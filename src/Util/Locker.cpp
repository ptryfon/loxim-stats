#include <Util/Locker.h>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include <sstream>

using namespace Util;
using namespace std;



Locker::Locker(pthread_mutex_t &mutex) : mutex(mutex)
{
	int error;
	if ((error = pthread_mutex_lock(&mutex))){
		stringstream ss;
		ss << "pthread_mutex_lock failed with code " << error << " (" <<
			strerror(error) << ")";
		throw runtime_error(ss.str());
	}
}


Locker::~Locker()
{
	assert(!pthread_mutex_unlock(&mutex));
}

void Locker::wait(pthread_cond_t &cond)
{
	int error;
	if ((error = pthread_cond_wait(&cond, &mutex))){
		stringstream ss;
		ss << "pthread_cond_wait failed with code " << error << " (" <<
			strerror(error) << ")";
		throw runtime_error(ss.str());
	}
}
