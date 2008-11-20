#ifndef ABORTER_H
#define ABORTER_H

#include <LoximClient/LoximClient.h>

namespace LoximClient{

	void *AB_starter(void *arg);

	class LoximClient;

	class Aborter{
		friend void *AB_starter(void *arg);
		public:
			Aborter(LoximClient *client);
			void start();
			void stop();
			void trigger();
		protected:
			pthread_mutex_t mutex;
			pthread_cond_t cond;
			pthread_t thread;
			int shutting_down;
			LoximClient *client;

			void loop();

	};
}

#endif /* ABORTER_H */
