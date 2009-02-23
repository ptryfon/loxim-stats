#ifndef ABORTER_H
#define ABORTER_H

#include <pthread.h>
#include <Util/Concurrency.h>

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
			Util::Mutex mutex;
			Util::CondVar cond;
			pthread_t thread;
			bool shutting_down;
			Client &client;

			void loop();

	};
}

#endif /* ABORTER_H */
