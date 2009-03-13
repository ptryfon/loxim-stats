#ifndef ABORTER_H
#define ABORTER_H

#include <pthread.h>
#include <Util/Concurrency.h>
#include <Util/SignalReceiver.h>

namespace Client{

	class Client;

	class Aborter : public Util::StartableSignalReceiver {
		public:
			Aborter(Client &client);
			void start();
			void kill(bool synchronous);
			void signal_handler(int sig);
		protected:
			Util::Mutex mutex;
			Util::CondVar cond;
			bool shutting_down;
			Client &client;
	};
}

#endif /* ABORTER_H */
