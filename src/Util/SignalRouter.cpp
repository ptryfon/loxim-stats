#include <signal.h>
#include <memory>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <Util/SignalRouter.h>
#include <Util/SignalReceiver.h>
#include <Util/Concurrency.h>

using namespace std;
using namespace Util;
using namespace Errors;

namespace Util{
	class Receiver{
		private:
			SignalReceiver &receiver;
			std::vector<int> signals;
			sigset_t sigmask;
		public:

			Receiver(SignalReceiver &receiver, sigset_t mask,
					const std::vector<int> &signals) : 
				receiver(receiver), signals(signals)
			{
				sigmask = mask;
			}

			void receive(int sig)
			{
				receiver.signal_handler(sig);
			}

			SignalReceiver &get_receiver()
			{
				return receiver;
			}

			const vector<int> &get_signals()
			{
				return signals;
			}

			const sigset_t &get_sigmask()
			{
				return sigmask;
			}

	};

	map<pthread_t, Receiver*> SignalRouter::map;
	Mutex SignalRouter::map_protector;
	
	void SignalRouter::signal_route(int i)
	{
		SignalRouter::route(i);
	}

	void SignalRouter::cleaner(void *arg)
	{
		Locker l(SignalRouter::map_protector);
		auto_ptr<Receiver> r(SignalRouter::map[pthread_self()]);
		SignalRouter::map.erase(pthread_self());
	}
	
	void *SignalRouter::starter(void *arg)
	{
		
		Receiver &r(*reinterpret_cast<Receiver*>(arg));
		struct sigaction sig;
		int error;
		SignalRouter::register_thread(pthread_self(), r);
		sig.sa_handler = SignalRouter::signal_route;
		sig.sa_flags = 0;
		for (vector<int>::const_iterator i = r.get_signals().begin(); i != r.get_signals().end(); ++i) {
			if (sigaction(*i, &sig, 0)){
				//TODO: do sth
			}
		}
		pthread_cleanup_push(SignalRouter::cleaner, NULL);
		if ((error = pthread_sigmask(SIG_SETMASK, &r.get_sigmask(), NULL))){
			//TODO: do something reasonable here!!!
		}
		//now that we've assured that signals will be properly handled,
		//we may start the thread's logic
		StartableSignalReceiver &recv = dynamic_cast<StartableSignalReceiver&>(r.get_receiver());
		try {
			recv.start();
		} catch ( ... ) {
			//XXX log it
		}
		pthread_cleanup_pop(1);
		return NULL;
	}

	void SignalRouter::register_thread(pthread_t thread, Receiver &recv)
	{
		Locker l(map_protector);
		if (map.find(thread) != map.end())
			throw LoximException(EINVAL);
		map[thread] = &recv;
	}

	void SignalRouter::register_thread(pthread_t thread, SignalReceiver &receiver)
	{
		vector<int> v;
		sigset_t mask;
		int error = pthread_sigmask(SIG_SETMASK, NULL, &mask);
		if (error)
			throw LoximException(error);
		Receiver *r = new Receiver(receiver, mask, v);
		register_thread(thread, *r);
	}
	
	void SignalRouter::unregister_thread(pthread_t thread)
	{
		Locker l(map_protector);
		Receiver *r = map[thread];
		map.erase(thread);
		delete r;
	}

	void SignalRouter::route(int sig)
	{
		//ignore signals for which a route does not exist
		if (map.find(pthread_self()) != map.end())
			map[pthread_self()]->receive(sig);	
	}

	void SignalRouter::spawn_and_register(StartableSignalReceiver
			&recv, const sigset_t &mask, const vector<int> &signals)
	{
		sigset_t block_mask, old_mask;
		int error;
		if (sigfillset(&block_mask))
			throw new LoximException(errno);
		auto_ptr<Receiver> r(new Receiver(recv, mask, signals));
		Masker m(block_mask);
		if ((error = pthread_create(&(recv.get_thread()), NULL, SignalRouter::starter, r.get()))) {
			throw LoximException(error);
		}
		r.release();
		//actually there should be some synchronisation to ensure that
		//the second initialisation part was also successful
	}
			
	void SignalRouter::spawn_and_register(StartableSignalReceiver& recv, int sig)
	{
		sigset_t mask;
		if (sigfillset(&mask))
			throw new LoximException(errno);
		if (sigdelset(&mask, sig))
			throw new LoximException(errno);
		vector<int> v;
		v.push_back(sig);
		spawn_and_register(recv, mask, v);
	}

	void SignalRouter::spawn_and_register(StartableSignalReceiver &recv)
	{
		sigset_t mask;
		if (sigfillset(&mask))
			throw new LoximException(errno);
		vector<int> v;
		spawn_and_register(recv, mask, v);
	}
	
}

