#include <iostream>
#include <cassert>
#include <cstdlib>

#include <Util/smartptr.h>
#include <Util/Thread.h>
#include <Util/Concurrency.h>
#include <Util/Misc.h>

using namespace std;
using namespace Util;
using namespace _smptr;

SpinLock lock;
int cntr = 0;

class TestThread : public Thread {
	public:

		TestThread(int nr) : nr(nr)
		{
		}

		void start()
		{
			cout << "Hello, I am thread " << nr << endl;
			int sum = 0;
			for (int i = 0; i < nr * 10000; ++i)
				sum += nr;
			cout << "I am thread " << nr << ", the magic number is " << sum << ", exiting" << endl;
			SpinLock::Locker l(lock);
			++cntr;
		}

		void signal_handler(int)
		{
			cout << "signal?!!!" << endl;
			abort();
		}
	private:
		int nr;
};

int main()
{
	const int threads = 100;
	{
		vector<shared_ptr<TestThread> > v;
		for (int i = 0; i < threads; ++i) {
			shared_ptr<TestThread> t(new TestThread(i));
			v.push_back(t);
			t->run();
		}
	}
	assert(cntr == threads);
	cout << "Success" << endl;
	return 0;
}
