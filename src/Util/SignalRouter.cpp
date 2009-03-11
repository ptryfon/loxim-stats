#include <signal.h>
#include <Util/SignalRouter.h>
#include <Util/Concurrency.h>
#include <Util/smartptr.h>

using namespace std;
using namespace Util;
using namespace _smptr;

namespace Util{

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
		
		Receiver *r = (Receiver*)arg;
		struct sigaction sig;
		int error;
		SignalRouter::register_thread(pthread_self(), r);
		sigemptyset(&sig.sa_mask);
		sig.sa_handler = SignalRouter::signal_route;
		sig.sa_flags = 0;
		if (sigaction(r->get_sig(), &sig, 0)){
			//TODO: do sth
		}
		pthread_cleanup_push(SignalRouter::cleaner, NULL);


		if ((error = pthread_sigmask(SIG_SETMASK, r->get_sigmask(), NULL))){
			//TODO: do something reasonable here!!!
		}
		//now that we've assured that signals will be properly handled,
		//we may start the thread's logic
		r->get_starter()(r->get_arg());
		pthread_cleanup_pop(1);
		return NULL;
	}

	void SignalRouter::register_thread(pthread_t thread, Receiver *recv)
	{
		map[thread] = recv;
	}

	void SignalRouter::register_thread(pthread_t thread, recv_fun_t recv, void *arg)
	{
		Receiver *r = new Receiver(recv, arg);
		Locker l(map_protector);
		map[thread] = r;
	}
	
	void SignalRouter::unregister_thread(pthread_t thread)
	{
		map.erase(thread);
	}

	void SignalRouter::route(int sig)
	{
		//ignore signals for which a route does not exist
		if (map.find(pthread_self()) != map.end())
			map[pthread_self()]->call(sig);	
	}

	int SignalRouter::spawn_and_register(pthread_t* thread, void *(*starter)(void*), recv_fun_t handler, void* arg, int sig)
	{
		sigset_t old_mask, new_mask;
		int error;
		if ((error = pthread_sigmask(0, NULL, &old_mask)))
			return error;
		new_mask = old_mask;
		Receiver *r = new Receiver(handler, starter, arg, sig, &old_mask);
		if ((error = sigaddset(&new_mask, sig))){
			delete r;
			return error;
		}
		if ((error = pthread_sigmask(SIG_SETMASK, &new_mask, NULL))){
			delete r;
			return error;
		}
		if ((error = pthread_create(thread, NULL, SignalRouter::starter, r))){
			delete r;
			return error;
		}
		pthread_sigmask(SIG_SETMASK, &old_mask, NULL);
		//actually there should be some synchronisation to ensure that
		//the second initialisation part was also successful
		return 0;
	}
	
	void Receiver::set_receiver(recv_fun_t receiver)
	{
		this->receiver = receiver;
	}
	
	recv_fun_t Receiver::get_receiver()
	{
		return receiver;
	}
	
	void Receiver::set_arg(void *arg)
	{
		this->arg = arg;
	}
	
	void *Receiver::get_arg()
	{
		return arg;
	}


	int Receiver::get_sig()
	{
		return sig;
	}

	void Receiver::set_sig(int sig)
	{
		this->sig = sig;
	}

	void Receiver::set_sigmask(sigset_t *mask)
	{
		this->sigmask = *mask;
	}

	sigset_t *Receiver::get_sigmask()
	{
		return &sigmask;
	}

	void Receiver::call(int sig)
	{
		receiver(pthread_self(), sig, arg);
	}

	void *(*Receiver::get_starter())(void*)
	{
		return starter;
	}
	
	Receiver::Receiver(recv_fun_t receiver, void *(*starter)(void*), void *arg, int sig, sigset_t *sigmask)
	{
		this->receiver = receiver;
		this->starter = starter;
		this->arg = arg;
		this->sig = sig;
		if (sigmask)
			this->sigmask = *sigmask;
	}
	
	Receiver::Receiver(const Receiver& r)
	{
		this->receiver = r.receiver;
		this->arg = r.arg;
		this->starter = r.starter;
		this->sigmask = r.sigmask;
		this->sig = r.sig;
	}
	
	Receiver::Receiver(recv_fun_t receiver, void *arg)
	{
		this->receiver = receiver;
		this->arg = arg;
	}


	Receiver &Receiver::operator=(const Receiver &r)
	{
		this->receiver = r.receiver;
		this->arg = r.arg;
		this->starter = r.starter;
		this->sigmask = r.sigmask;
		this->sig = r.sig;
		return *this;
	}
}

