#include <pthread.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include <Errors/Exceptions.h>
#include <Util/InfLoopThread.h>
#include <Util/Concurrency.h>
#include <Util/SignalRouter.h>
#include <Util/SignalReceiver.h>

using namespace Util;
using namespace Errors;
using namespace std;

class InfLogic : public StartableSignalReceiver {
	private:
		Mutex m;
		CondVar c;
	public:
		void signal_handler(int sig)
		{
		}

		
		void kill()
		{
			cout << "kill called" << endl;
			Locker l(m);
			cout << "locked in killer" << endl;
			c.signal();
			cout << "after signal" << endl;
		}

		void init()
		{
			cout << "lock in init" << endl;
			m.lock();
			cout << "locked in init" << endl;
		}

		void start()
		{
			try {
				cout << "new thread (falling asleep)" << endl;
				c.wait(m);
				cout << "after wait" << endl;
				m.unlock();
				cout << "exiting" << endl;
			} catch (LoximException &ex) {
				cout << "exception caught" << endl;
				cout << ex.to_string() << endl;
			}
		}
};

int main(int argc, char ** argv)
{
	sigset_t mask;
	sigfillset(&mask);
	vector<int> v;
	InfLoopThread<InfLogic> t(auto_ptr<InfLogic>(new InfLogic()), mask, v);
	cout << "should still be working :)" << endl;
	return 0;
}
