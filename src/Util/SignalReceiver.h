#ifndef SIGNAL_RECEIVER_H
#define SIGNAL_RECEIVER_H

#include <pthread.h>
#include <Util/Misc.h>

namespace Util {
	class SignalReceiver : private NonValue {
		protected:
			//not very pretty
			pthread_t thread;

		public:
			virtual ~SignalReceiver();
			pthread_t &get_thread() { return thread; }

			virtual void signal_handler(int sig) = 0;
			//this is guaranteed to be called with all signals not
			//yet unmasked
			virtual void init() { };

	};

	class StartableSignalReceiver : public SignalReceiver {
		public:
			virtual void start() = 0;
	};
}

#endif /* SIGNAL_RECEIVER_H */
