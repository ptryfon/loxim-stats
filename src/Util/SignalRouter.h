#ifndef SIGNAL_ROUTER_H
#define SIGNAL_ROUTER_H

#include <map>
#include <vector>
#include <pthread.h>
#include <signal.h>
#include <Util/Concurrency.h>
#include <Util/smartptr.h>


namespace Util{

	class SignalReceiver;
	class StartableSignalReceiver;
	class Receiver;
	

	class SignalRouter{
		private:
			static std::map<pthread_t, 
				_smptr::shared_ptr<Receiver> > map;
			static Mutex map_protector;
			static void signal_route(int i);
			static void register_thread(pthread_t,
					_smptr::shared_ptr<Receiver>);
			static void *starter(void *arg);
			static void cleaner(void *arg);
			static void route(int sig);
		public:
			static void unregister_thread(pthread_t);
			static void register_thread(pthread_t,
					SignalReceiver&);
			static void spawn_and_register(
					StartableSignalReceiver&, const
					sigset_t&, const std::vector<int>&);

			/* the same as the above with sigmask masking
			 * everything except sig and signals beging a vector
			 * with just sig */
			static void spawn_and_register(
					StartableSignalReceiver&, int sig);

			/* another variant with all signals blocked */
			static void spawn_and_register(
					StartableSignalReceiver&);
	};


}

#endif /* SIGNAL_ROUTER_H */
