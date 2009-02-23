#ifndef INF_LOOP_THREAD_H
#define INF_LOOP_THREAD_H

#include <pthread.h>

namespace Util {
	class InfLoopThread {
		private:
			static void* starter(void *);
			void pre_loop();
		protected:
			bool shutting_down;
			bool alive;
			pthread_t thread;
			pthread_mutex_t loop_mutex;
			pthread_mutex_t loop_cond;
			virtual void loop() = 0;
		public:
			InfLoopThread();
			virtual ~InfLoopThread();

			/* kill the thread in order to be sure that destructor
			 * won't block */
			virtual void kill(bool synchronous) = 0;

			bool is_alive();


	};
}

#endif /* INF_LOOP_THREAD_H */
