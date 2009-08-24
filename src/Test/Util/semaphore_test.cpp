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

volatile int val_a = 0, val_b = 0;
RWSemaphore sem;

//there is 4 times more readers than writers
const int reader_iter = 45000;
const int writer_iter = 14000;
const int threads = 50;

class TestThread : public Thread {
	public:

		TestThread(int sleeptime, int nr, bool reader) : nr(nr), reader(reader),
			sleeptime(sleeptime)
		{
		}

		void start()
		{
			const int n = reader?reader_iter:writer_iter;
//			cout << (reader?"reader":"writer") << endl;
			for (int i = 0; i < n; ++i) {
				if (reader) {
					sem.lock_read();
//					cout << "check!" << endl;
					if (val_a != val_b) {
						cout << "values differ!" << endl;
						abort();
					}
				} else {
					sem.lock_write();
//					cout << "inc!" << endl;
					val_a++;
					val_b++;
				}
				if (sleeptime)
					usleep(random()%sleeptime);
				sem.unlock();
//				cout << "released!" << endl;
			}
		}

		void signal_handler(int)
		{
		}
	private:
		int nr;
		bool reader;
		int sleeptime;
};

void test(int sleeptime)
{
	val_a = val_b = 0;
	int writer_threads = 0;
	{
		vector<shared_ptr<TestThread> > v;
		for (int i = 0; i < threads; ++i) {
			if (i % 5 == 0)
				++writer_threads;
			shared_ptr<TestThread> t(new TestThread(sleeptime, i, i % 5));
			v.push_back(t);
			t->run();
		}
	}
	if (val_a != val_b || val_a != writer_iter * writer_threads) {
		cout << "wrong checksum: (" << val_a << "), should be " << writer_iter  * writer_threads << endl;
		abort();
	}
	cout << "Success" << endl;
}

int main()
{
	test(0);
//	test(1000);
	return 0;
}
