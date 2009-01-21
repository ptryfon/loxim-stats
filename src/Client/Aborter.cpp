#include <Client/Aborter.h>
#include <Client/Client.h>
#include <Util/Locker.h>

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
		pthread_mutex_init(&mutex, 0);
		pthread_cond_init(&cond, 0);
	}

	void Aborter::start()
	{
		pthread_create(&thread, NULL, AB_starter, this);
	}

	void Aborter::stop()
	{
		Locker l(mutex);
		shutting_down = true;
		pthread_cond_signal(&cond);
	}

	/**
	 * The idea is to make sending an abort non-blocking. If we're in the
	 * middle of sending an abort, it won't do anything.
	 */
	void Aborter::trigger()
	{
		pthread_cond_signal(&cond);
	}

	void Aborter::loop()
	{
		Locker l(mutex);
		while (true){
			pthread_cond_wait(&cond, &mutex);
			if (shutting_down)
				pthread_exit(NULL);
			client.abort();
		}
	}

}
