#include "Aborter.h"

namespace LoximClient{

	void *AB_starter(void *arg)
	{
		((Aborter*)arg)->loop();
		return NULL;
	}

	Aborter::Aborter(LoximClient *client)
	{
		this->client = client;
		this->shutting_down = 0;
		pthread_mutex_init(&mutex, 0);
		pthread_cond_init(&cond, 0);
	}

	void Aborter::start()
	{
		pthread_create(&thread, NULL, AB_starter, this);
	}

	void Aborter::stop()
	{
		pthread_mutex_lock(&mutex);
		shutting_down = 1;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
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
		pthread_mutex_lock(&mutex);
		while (true){
			pthread_cond_wait(&cond, &mutex);
			if (shutting_down)
				pthread_exit(NULL);
			client->abort();
		}
	}

}
