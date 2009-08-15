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
		Mutex::Locker *l;
	public:
		void signal_handler(int)
		{
		}

		
		void kill()
		{
			cout << "kill called" << endl;
			Mutex::Locker l(m);
			cout << "locked in killer" << endl;
			c.signal();
			cout << "after signal" << endl;
		}

		void init()
		{
			cout << "lock in init" << endl;
			l = new Mutex::Locker(m);
			cout << "locked in init" << endl;
		}

		void start()
		{
			try {
				cout << "new thread (falling asleep)" << endl;
				l->wait(c);
				cout << "after wait" << endl;
				delete l;
				cout << "exiting" << endl;
			} catch (LoximException &ex) {
				cout << "exception caught" << endl;
				cout << ex.to_string() << endl;
			}
		}
};

int main(const int, const char* const* const)
{
	sigset_t mask;
	sigfillset(&mask);
	vector<int> v;
	InfLoopThread<InfLogic> t(auto_ptr<InfLogic>(new InfLogic()), mask, v);
	cout << "should still be working :)" << endl;
	return 0;
}
