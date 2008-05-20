#ifndef KEEPALIVE_H
#define KEEPALIVE_H

#include <pthread.h>
#include "LoximSession.h"
#include <Errors/ErrorConsole.h>



namespace LoximServer{
	class LoximSession;

	class KeepAliveThread{
		private:
			friend void *thread_starter(void*);
			bool running;
			bool shutting_down;
			pthread_cond_t cond;
			pthread_mutex_t cond_mutex;
			void main_loop();
			LoximSession *session;
			pthread_t thread;
			ErrorConsole *err_cons;
			int error;
			bool answer_received;
		public:
			KeepAliveThread(LoximSession *session, ErrorConsole *err_cons);
			int start();
			int shutdown();
			void set_answered();
	};
}



#endif
