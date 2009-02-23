#include <Client/Aborter.h>
#include <Client/Client.h>

using namespace Util;

namespace Client{

	void *AB_starter(void *arg)
	{
		((Aborter*)arg)->loop();
		return NULL;
	}

	Aborter::Aborter(Client &client) :
		shutting_down(false), client(client)
	{
	}

	void Aborter::start()
	{
		pthread_create(&thread, NULL, AB_starter, this);
	}

	void Aborter::stop()
	{
		Locker l(mutex);
		shutting_down = true;
		cond.signal();
	}

	/**
	 * The idea is to make sending an abort non-blocking. If we're in the
	 * middle of sending an abort, it won't do anything.
	 */
	void Aborter::trigger()
	{
		cond.signal();
	}

	void Aborter::loop()
	{
		Locker l(mutex);
		while (true){
			l.wait(cond);
			if (shutting_down)
				pthread_exit(NULL);
			client.abort();
		}
	}

}
