#include <signal.h>
#include <memory>
#include <cassert>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <Util/SignalRouter.h>
#include <Util/SignalReceiver.h>
#include <Util/Concurrency.h>
#include <Util/Backtrace.h>
#include <iostream>

using namespace std;
using namespace Util;
using namespace Errors;
using namespace _smptr;

namespace Util{
	class Receiver{
		public:
			SignalReceiver &receiver;
			std::vector<int> signals;
			sigset_t sigmask;
			int error;
			Mutex mutex;
			CondVar cond;

			Receiver(SignalReceiver &receiver, sigset_t mask,
					const std::vector<int> &signals) : 
				receiver(receiver), signals(signals)
			{
				sigmask = mask;
			}

			~Receiver()
			{
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

	map<pthread_t, shared_ptr<Receiver> > SignalRouter::map;
	Mutex SignalRouter::map_protector;
	
	void SignalRouter::signal_route(int i)
	{
		SignalRouter::route(i);
	}

	void SignalRouter::cleaner(void *arg)
	{
		Locker l(SignalRouter::map_protector);
		SignalRouter::map.erase(pthread_self());
	}
	
	void *SignalRouter::starter(void *arg)
	{
		shared_ptr<Receiver> r(
			*(reinterpret_cast<shared_ptr<Receiver>*>(arg)));
		try {
			SignalRouter::register_thread(r);
		} catch (...) {
			if (!r->error)
				r->error = EIO;
			r->cond.signal();
		}
		{
			//now that we've assured that signals will be properly handled,
			//we may start the thread's logic
			StartableSignalReceiver &recv =
				dynamic_cast<StartableSignalReceiver&>(r->get_receiver());
			try {
				recv.init();
			} catch ( ... ) {
				r->error = EINVAL;
				r->cond.signal();
				SignalRouter::cleaner(NULL);
				return NULL;
			}
			if ((r->error = pthread_sigmask(SIG_SETMASK, &r->get_sigmask(),
							NULL))){
				r->cond.signal();
				SignalRouter::cleaner(NULL);
				return NULL;
			}
			{
				Locker l(r->mutex);
				r->cond.signal();
			}
			try {
				recv.start();
			} catch ( ... ) {
				//XXX log it
			}
			SignalRouter::cleaner(NULL);
		}
		return NULL;
	}

	//it has side effects - modifies r->error
	void SignalRouter::register_thread(shared_ptr<Receiver> r)
	{
		pthread_t thread = pthread_self();
		Locker l(map_protector);
		if (map.find(thread) != map.end())
			throw LoximException(EINVAL);
		map[thread] = r;
		struct sigaction sig;
		sig.sa_handler = SignalRouter::signal_route;
		sig.sa_flags = 0;
		for (vector<int>::const_iterator i = r->get_signals().begin();
				i != r->get_signals().end(); ++i) {
			if ((r->error = sigaction(*i, &sig, 0))){
				throw LoximException(r->error);
			}
		}
	}

	void SignalRouter::register_thread(SignalReceiver &receiver, const
			vector<int> &sigs)
	{
		sigset_t mask;
		int error = pthread_sigmask(SIG_SETMASK, NULL, &mask);
		if (error)
			throw LoximException(error);
		register_thread(shared_ptr<Receiver>(new Receiver(receiver,
						mask, sigs)));
	}
	
	void SignalRouter::register_thread(SignalReceiver &receiver, int sig)
	{
		vector<int> v;
		v.push_back(sig);
		register_thread(receiver, v);
	}
	
	void SignalRouter::unregister_thread(pthread_t thread)
	{
		Locker l(map_protector);
		map.erase(thread);
	}

	void SignalRouter::route(int sig)
	{
		//ignore signals for which a route does not exist
		if (map.find(pthread_self()) != map.end())
			map[pthread_self()]->receive(sig);	
	}

	void SignalRouter::spawn_and_register(StartableSignalReceiver
			&recv, const sigset_t &mask, const vector<int>
			&signals)
	{
		sigset_t block_mask;
		int lerror;
		if (sigfillset(&block_mask))
			throw new LoximException(errno);
		shared_ptr<Receiver> r(new Receiver(recv, mask, signals));
		auto_ptr<shared_ptr<Receiver> > rp(new
				shared_ptr<Receiver>(r));
		Locker l(r->mutex);
		{
			Masker m(block_mask);
			if ((lerror = pthread_create(&(recv.get_thread()),
							NULL,
							SignalRouter::starter,
							rp.get()))) {
				throw LoximException(lerror);
			}

		}
		l.wait(r->cond);
		if (r->error)
			throw LoximException(r->error);
	}
			
	void SignalRouter::spawn_and_register(StartableSignalReceiver& recv,
			int sig)
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

