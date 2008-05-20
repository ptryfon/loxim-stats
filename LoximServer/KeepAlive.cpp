#include "KeepAlive.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <protocol/packages/ASCPingPackage.h>

namespace LoximServer{

	void *thread_starter(void *arg)
	{
		((KeepAliveThread*)arg)->main_loop();
		return NULL;
	}
	
	KeepAliveThread::KeepAliveThread(LoximSession *session, ErrorConsole *err_cons)
	{
		this->session = session;
		pthread_mutex_init(&(this->cond_mutex), 0);
		pthread_cond_init(&(this->cond), 0);
		this->shutting_down = false;
		this->err_cons = err_cons;
		this->error = 0;
	}

	int KeepAliveThread::start()
	{
		return pthread_create(&thread, NULL, thread_starter, this);
	}

	int KeepAliveThread::shutdown()
	{
		shutting_down = 1;
		pthread_cond_signal(&cond);
		pthread_join(thread, NULL);
		return error;
	}

	void KeepAliveThread::main_loop()
	{
		err_cons->printf("KeepAliveThred::main_loop starts\n");
		struct timeval now;
		struct timespec tout;
		int res;
		pthread_mutex_lock(&cond_mutex);
		answer_received = true;
		do {
			gettimeofday(&now, 0);
			tout.tv_sec = now.tv_sec + session->get_server()->get_config_keep_alive_interval();
			tout.tv_nsec = now.tv_usec * 1000;
			res = pthread_cond_timedwait(&cond, &cond_mutex, &tout);
			if (!shutting_down && res == ETIMEDOUT){
				//actual action
				err_cons->printf("Keepalive check\n");
				if (!answer_received){
					err_cons->printf("Keepalive thread has just discovered client death.\n");
					session->shutdown();
					pthread_exit(0);
				} else {
					answer_received = false;
					pthread_mutex_lock(&(session->send_mutex));
					ASCPingPackage package;
					if (!session->layer0->writePackage(&package))
						res = ESend;
					pthread_mutex_unlock(&(session->send_mutex));
				}
				
			}
		} while (!shutting_down && (res == ETIMEDOUT || res == 0));
		if (!shutting_down)
			err_cons->printf("Warning: keepalive thread died\n");
		else
			res = 0;
		error = res;
		pthread_mutex_unlock(&cond_mutex);
		pthread_exit(NULL);
	}

	void KeepAliveThread::set_answered()
	{
		answer_received = true;
	}

}

