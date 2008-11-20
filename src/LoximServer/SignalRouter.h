#ifndef SIGNAL_ROUTER_H
#define SIGNAL_ROUTER_H

#include <map>
#include <pthread.h>

using namespace std;

namespace LoximServer{

	typedef void (*recv_fun_t)(pthread_t, int, void *);
	void *SR_starter(void *arg);
	void SR_cleaner(void *arg);
	class Receiver{
		private:
			recv_fun_t receiver;
			void *arg;
			void *(*starter)(void*);
			sigset_t sigmask;
			int sig;
		public:
			void set_receiver(recv_fun_t);
			recv_fun_t get_receiver();
			void set_arg(void*);
			void *get_arg();
			void set_sigmask(sigset_t *sigset);
			void set_sig(int sig);
			void *(*get_starter())(void*);
			int get_sig();
			sigset_t *get_sigmask();
			void call(int);
			Receiver(recv_fun_t, void *(*)(void*),  void*, int sig, sigset_t *);
			Receiver(const Receiver&);
			Receiver &operator=(const Receiver &);
			Receiver(recv_fun_t, void*);
	};

	class SignalRouter{
		private:
			static map<pthread_t, Receiver*> map;
		public:
			static void register_thread(pthread_t, Receiver*);
			static void register_thread(pthread_t, recv_fun_t handler, void *arg);
			static void unregister_thread(pthread_t);
			static void route(int sig);
			static int spawn_and_register(pthread_t*, void *(*starter)(void*), recv_fun_t handler, void* arg, int sig);
		friend void SR_cleaner(void *arg);
		friend void *SR_starter(void *arg);
	};


}

#endif /* SIGNAL_ROUTER_H */
