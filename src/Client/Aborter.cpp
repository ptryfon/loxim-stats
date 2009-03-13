#include <Client/Aborter.h>
#include <Client/Client.h>

using namespace Util;

namespace Client{


	Aborter::Aborter(Client &client) :
		shutting_down(false), client(client)
	{
	}

	void Aborter::kill(bool synchronous)
	{
		Locker l(mutex);
		shutting_down = true;
		cond.signal();
	}

	/**
	 * The idea is to make sending an abort non-blocking. If we're in the
	 * middle of sending an abort, it won't do anything.
	 */
	void Aborter::signal_handler(int sig)
	{
		cond.signal();
	}

	void Aborter::start()
	{
		try{
			Locker l(mutex);
			while (!shutting_down){
				l.wait(cond);
				if (shutting_down)
					pthread_exit(NULL);
				client.abort();
			}
		} catch (...) {
			//XXX log it
		}
	}

}
