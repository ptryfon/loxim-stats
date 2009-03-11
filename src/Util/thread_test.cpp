#include <Util/InfLoopThread.h>
#include <Util/Concurrency.h>
#include <iostream>

using namespace Util;
using namespace std;

class InfLogic {
	private:
		Mutex m;
		CondVar c;
	public:
		void kill(bool synchronous)
		{
			c.signal();
		}

		void on_exit()
		{
			cout << "exiting" << endl;
		}

		void loop()
		{
			cout << "new thread" << endl;
			Locker l(m);
			l.wait(c);
		}
};

int main(int argc, char ** argv)
{
	PthreadInfLoopStarter s;
	InfLoopThread<InfLogic> t(auto_ptr<InfLogic>(new InfLogic()), s);
	cout << t.is_alive() << endl;
	return 0;
}
