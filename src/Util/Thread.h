#ifndef UTIL_THREAD_H
#define UTIL_THREAD_H

#include <pthread.h>
#include <cassert>
#include <memory>
#include <vector>
#include <Errors/Exceptions.h>
#include <Util/Concurrency.h>
#include <Util/Misc.h>
#include <Util/SignalRouter.h>
#include <Util/SignalReceiver.h>

namespace Util {

	class Thread : public StartableSignalReceiver {
		private:
			Mutex mutex;
			bool joined;
		public:
			Thread();
			~Thread();
			virtual void signal_handler(int i);
			virtual void start() = 0;
			virtual void init();
			void run();
			void run(StartableSignalReceiver &recv, const sigset_t &mask, const
					std::vector<int> &sigs);
			void join();

	};
}

#endif /* UTIL_THREAD_H */
