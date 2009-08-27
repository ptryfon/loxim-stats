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
RWSemaphore *sem;

//there is 4 times more readers than writers
int reader_iter;
int writer_iter;
int threads;

class TestThread : public Thread {
	public:

		TestThread(int sleeptime, int nr, bool reader, bool upgrade) : nr(nr), reader(reader),
			sleeptime(sleeptime), upgrade(upgrade)
		{
		}

		void start()
		{
			const int n = reader?reader_iter:writer_iter;
			for (int i = 0; i < n; ++i) {
				if (reader) {
					sem->lock_read();
					if (val_a != val_b) {
						cout << "values differ!" << endl;
						abort();
					}
					RWUJSemaphore *sem2 = dynamic_cast<RWUJSemaphore*>(sem);
					if (upgrade && sem2) {
						while (!sem2->lock_upgrade(nr)) {
							sem2->lock_read();
							if (val_a != val_b) {
								abort();
							}
						}
						val_a++;
						val_b++;
					}
				} else {
					sem->lock_write();
					val_a++;
					val_b++;
				}
				if (sleeptime)
					usleep(random()%sleeptime);
				sem->unlock();
			}
		}

		void signal_handler(int)
		{
		}
	private:
		int nr;
		bool reader;
		int sleeptime;
		bool upgrade;

};

void test(int sleeptime)
{
	val_a = val_b = 0;
	int writer_threads = 0;
	int upgrader_threads = 0;
	{
		vector<shared_ptr<TestThread> > v;
		for (int i = 0; i < threads; ++i) {
			if (i % 5 == 0)
				++writer_threads;
			else if (i % 9 == 0)
				++upgrader_threads;
			shared_ptr<TestThread> t(new TestThread(sleeptime, i, i % 5, (i % 9 == 0)));
			v.push_back(t);
			t->run();
		}
	}
	const int proper_val = writer_threads * writer_iter + ((dynamic_cast<RWUJSemaphore*>(sem) != NULL)?(upgrader_threads * reader_iter):0);
	if (val_a != val_b || val_a != proper_val) {
		cout << "wrong checksum: (" << val_a << "), should be " << proper_val << endl;
		abort();
	}
	cout << "Success" << endl;
}

int main()
{
	sem = new RWSemaphore();
	reader_iter = 45000;
	writer_iter = 14000;
	threads = 20;
	test(0);
	reader_iter = 450;
	writer_iter = 140;
	threads = 20;
	test(1000);
	delete sem;
	sem = new RWUJSemaphore();
	reader_iter = 45000;
	writer_iter = 14000;
	threads = 20;
	test(0);
	reader_iter = 450;
	writer_iter = 140;
	threads = 20;
	test(1000);
	delete sem;
	return 0;
}
