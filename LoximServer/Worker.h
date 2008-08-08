#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>
#include <LoximSession.h>

namespace LoximServer{
	void *W_starter(void *arg);

	class Worker{
		friend void *W_starter(void *arg);
		protected:
			pthread_t thread;
			pthread_mutex_t mutex;
			int shutting_down;
			LoximSession *session;
			pthread_cond_t idle_cond, completion_cond;
			Package *cur_package;
			ErrorConsole *err_cons;

			void start_continue();
			int process_package(Package *package,int *error);
		public:
			enum sub_ret_t {
				SUBM_SUCCESS = 1, /* package submitted */
				SUBM_EXIT,        /* package turned out to be bye, so we should exit */
				SUBM_ERROR        /* the package shouldn't have arrived now */
			};

			Worker(LoximSession *session, ErrorConsole *err_cons);
			void start();
			void cancel_job(bool synchrounous, bool mutex_locked);

			/**
			 * always synchronous, kills the thread
			 */
			void stop();

			/**
			 * 0 - submitted
			 * 1 - work in progress, error
			 */
			int submit(Package *package);
	};
}

#endif
