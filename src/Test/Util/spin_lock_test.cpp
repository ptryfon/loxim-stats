#include <iostream>
#include <cassert>
#include <cstring>
#include <pthread.h>
#include <stdlib.h>

#include <Util/Concurrency.h>

using namespace std;
using namespace Util;

volatile int k = 0;
SpinLock lock;

#define ITERATIONS 100000000

void *incrementer(void *)
{
	cerr << "Thread started" << endl;
	for (int i = 0; i < ITERATIONS; ++i) {
		SpinLock::Locker l(lock);
		volatile int j = k;
		j++;
		k = j;
	}
	return NULL;
}

int main(const int argc, const char *const *const argv)
{
	SpinLock l;
	int err;
	pthread_t thread1 = 0, thread2 = 0;
	{
		SpinLock::Locker l(lock);
		err = pthread_create(&thread1, NULL, incrementer, NULL);
		assert(err == 0);
		err = pthread_create(&thread2, NULL, incrementer, NULL);
		assert(err == 0);
	}
	if ((err = pthread_join(thread1, NULL))) {
		cout << "error: " << err << " (" << strerror(err) << ")" << endl;
		abort();
	}
	if ((err = pthread_join(thread2, NULL))) {
		cout << "error: " << err << " (" << strerror(err) << ")" << endl;
		abort();
	}
	assert(k == ITERATIONS * 2);
	cerr << "Success" << endl;
	return 0;
}
