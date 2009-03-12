#ifndef INF_LOOP_THREAD_H
#define INF_LOOP_THREAD_H

#include <pthread.h>
#include <cassert>
#include <Errors/Exceptions.h>
#include <Util/Concurrency.h>

namespace Util {

	template<class T>
	class InfLoopStarter {
		int start(pthread_t *thread, void **starter(void*), void *arg) const;
	};

	class PthreadInfLoopStarter {
	public:
		int start(pthread_t *thread, void *(starter(void*)), void *arg) const
		{
			return pthread_create(thread, 0, starter, arg);
		}

		void join(pthread_t thread)
		{
			pthread_join(thread, NULL);
		}
	};

	class InfLoopLogic {
		public:
			void kill(bool synchronous);
			void on_exit();
			void loop();
	};

	template <class T>
	class InfLoopThread {
		private:
			static void* starter(void *arg)
			{
				InfLoopThread<T> *thread_obj =
					reinterpret_cast<InfLoopThread<T>*>(arg);
				assert(thread_obj != NULL);
				thread_obj->alive = true;
				try {
					thread_obj->logic->loop();
				} catch (...) {
					//XXX some logging here
				}
				thread_obj->alive = false;
				try{
					thread_obj->logic->on_exit();
				} catch (...) {
					//XXX some logging here
				}
				return NULL;
			}
		protected:
			bool alive;
			pthread_t thread;
			auto_ptr<T> logic;
			Mutex start_mutex;
			CondVar start_cond;
		public:
			template <class Starter>
			InfLoopThread(auto_ptr<T> logic, const Starter &st) :
				alive(false), logic(logic)
			{
				int error = st.start(&thread,
						InfLoopThread<T>::starter,
						this);	
				if (error)
					//throw Errors::LoximException(error);
					{}
			}
			~InfLoopThread()
			{
				kill(true);
			}

			bool is_alive()
			{
				return alive;
			}

			
			void kill(bool synchronous)
			{
				logic->kill(synchronous);
				if (synchronous && alive)
					pthread_join(thread, 0);
			}

	};
}

#endif /* INF_LOOP_THREAD_H */
