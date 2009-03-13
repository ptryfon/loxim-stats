#ifndef INF_LOOP_THREAD_H
#define INF_LOOP_THREAD_H

#include <pthread.h>
#include <cassert>
#include <memory>
#include <Errors/Exceptions.h>
#include <Util/Concurrency.h>
#include <Util/SignalRouter.h>

namespace Util {

	template <class T>
	class InfLoopThread {
		protected:
			auto_ptr<T> logic;
			bool killed;
			Mutex mutex;
		public:
			void signal_handler(int i)
			{
				logic->handle_signal(i);
			}

			InfLoopThread(auto_ptr<T> logic, const sigset_t &mask,
					const vector<int> &signals) :
				logic(logic), killed(false)
			{
				SignalRouter::spawn_and_register(*(this->logic.get()),
						mask, signals);
			}
			InfLoopThread(auto_ptr<T> logic, int sig) :
				logic(logic), killed(false)
			{
				SignalRouter::spawn_and_register(*(this->logic.get()),
						sig);
			}
			~InfLoopThread()
			{
				kill(true);
			}

			void kill(bool synchronous)
			{
				Locker l(mutex);
				if (!killed) {
					logic->kill(synchronous);
					if (synchronous){
						pthread_join(logic->get_thread(),
								0);
					}
				}
			}

			T &get_logic()
			{
				return *logic;
			}

	};
}

#endif /* INF_LOOP_THREAD_H */
