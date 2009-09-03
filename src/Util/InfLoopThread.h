#ifndef INF_LOOP_THREAD_H
#define INF_LOOP_THREAD_H

#include <pthread.h>
#include <signal.h>
#include <cassert>
#include <memory>
#include <vector>
#include <Errors/Exceptions.h>
#include <Util/Concurrency.h>
#include <Util/SignalRouter.h>

namespace Util {

	template <class T>
	class InfLoopThread {
		protected:
			std::auto_ptr<T> logic;
			bool killed;
			Mutex mutex;
		public:
			void signal_handler(int i)
			{
				logic->handle_signal(i);
			}

			InfLoopThread(std::auto_ptr<T> logic, const sigset_t &mask,
					const std::vector<int> &signals) :
				logic(logic), killed(false)
			{
				SignalRouter::spawn_and_register(*(this->logic.get()),
						mask, signals);
			}

			InfLoopThread(std::auto_ptr<T> logic, int sig) :
				logic(logic), killed(false)
			{
				SignalRouter::spawn_and_register(*(this->logic.get()),
						sig);
			}
			~InfLoopThread()
			{
				kill();
				pthread_join(logic->get_thread(), 0);
			}

			void kill()
			{
				Mutex::Locker l(mutex);
				if (!killed) {
					killed = true;
					logic->kill();
				}
			}

			T &get_logic()
			{
				return *logic;
			}

	};
}

#endif /* INF_LOOP_THREAD_H */
