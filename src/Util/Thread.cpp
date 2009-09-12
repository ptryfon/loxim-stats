#include <Util/Thread.h>
#include <Errors/Exceptions.h>

using namespace Errors;

namespace Util {

	Thread::Thread() : joined(true)
	{
	}


	void Thread::init()
	{
		joined = false;
	}

	void Thread::run()
	{
		SignalRouter::spawn_and_register(*this);
	}

	void Thread::run(StartableSignalReceiver &recv, const sigset_t &mask, const
			vector<int>	&signals)
	{
		SignalRouter::spawn_and_register(*this,	mask, signals);
	}

	void Thread::signal_handler(int)
	{
	}

	void Thread::join()
	{
		Mutex::Locker l(mutex);
		if (!joined) {
			int err = pthread_join(get_thread(), NULL);
			if (err)
				throw LoximException(err);
		}
		joined = true;
	}

	Thread::~Thread()
	{
		try {
			join();
		} catch ( ... ) {
			//XXX log it
		}
	}

}

