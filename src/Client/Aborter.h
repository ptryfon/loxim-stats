#ifndef ABORTER_H
#define ABORTER_H

#include <pthread.h>

namespace Client{

	void *AB_starter(void *arg);

	class Client;

	class Aborter{
		friend void *AB_starter(void *arg);
		public:
			Aborter(Client &client);
			void start();
			void stop();
			void trigger();
		protected:
			pthread_mutex_t mutex;
			pthread_cond_t cond;
			pthread_t thread;
			bool shutting_down;
			Client &client;

			void loop();

	};
}

#endif /* ABORTER_H */
