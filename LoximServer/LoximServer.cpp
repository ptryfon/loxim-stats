#include <errno.h>
#include "LoximServer.h"
#include "LoximSession.h"
#include <protocol/sockets/TCPIPServerSocket.h>
#include <string>
#include <list>
#include <iostream>
#include <SignalRouter.h>

using namespace std;

namespace LoximServer{

	LoximServer::LoximServer(string hostname, int port, SBQLConfig *config)
	{
		this->prepared = 0;
		this->shutting_down = 0;
		this->hostname = hostname;
		this->port = port;
		this->socket = 0;
		this->err_cons = new ErrorConsole("LoximServer");
		if (config->getInt(CONFIG_ACCEPT_INTERVAL_NAME, this->config_accept_interval))
			this->config_accept_interval = CONFIG_ACCEPT_INTERVAL_DEFAULT;
		if (config->getInt(CONFIG_READ_INTERVAL_NAME, this->config_read_interval))
			this->config_read_interval = CONFIG_READ_INTERVAL_DEFAULT;
		if (config->getInt(CONFIG_MAX_SESSIONS_NAME, this->config_max_sessions))
			this->config_max_sessions = CONFIG_MAX_SESSIONS_DEFAULT;
		if (config->getInt(CONFIG_MAX_PACKAGE_SIZE_NAME, this->config_max_package_size))
			this->config_max_package_size = CONFIG_MAX_PACKAGE_SIZE_DEFAULT;
		if (config->getBool(CONFIG_AUTH_TRUST_ALLOWED_NAME, this->config_auth_trust_allowed))
			this->config_auth_trust_allowed = CONFIG_AUTH_TRUST_ALLOWED_DEFAULT;
		if (config->getInt(CONFIG_KEEP_ALIVE_INTERVAL_NAME, this->config_keep_alive_interval))
			this->config_keep_alive_interval = CONFIG_KEEP_ALIVE_INTERVAL_DEFAULT;
		pthread_mutex_init(&this->open_sessions_mutex, NULL);
	}

	LoximServer::~LoximServer()
	{
		pthread_mutex_destroy(&this->open_sessions_mutex);
		if (socket)
			delete socket;
		delete err_cons;
	}

	int LoximServer::prepare()
	{
		thread = pthread_self();
		SignalRouter::register_thread(thread, LSrv_signal_handler, this);
		socket = new TCPIPServerSocket(const_cast<char*>(hostname.c_str()), port);
		int res;
		if ((res = socket->bind()) == SOCKET_CONNECTION_STATUS_OK){
			prepared = 1;
			return 0;
		} else
			return res; 
	}

	int LoximServer::main_loop()
	{
		AbstractSocket *session_socket;
		if (!prepared)
			return -1;
		sigset_t sigset_tmpl, sigset;
		pthread_sigmask(0, NULL, &sigset_tmpl);
		sigaddset(&sigset_tmpl, SIGUSR1);
		sigset = sigset_tmpl;
		while (!shutting_down && (session_socket = socket->accept(&sigset, &shutting_down))){
			if (!session_socket)
				continue;
			pthread_mutex_lock(&open_sessions_mutex);
			if (!shutting_down && get_sessions_count() < get_config_max_sessions()){
				LoximSession *new_session = new LoximSession(this, session_socket, err_cons);
				open_sessions.insert(new_session);
				new_session->start();
			} else{
				session_socket->close();
				delete session_socket;
			}
			pthread_mutex_unlock(&open_sessions_mutex);
			sigset = sigset_tmpl;
		}
		pthread_mutex_lock(&open_sessions_mutex);
		err_cons->printf("Telling sessions to shutdown\n");
		for (set<LoximSession *>::iterator i = open_sessions.begin(); i != open_sessions.end(); i++){
			(*i)->shutdown();
		}
		pthread_mutex_unlock(&open_sessions_mutex);
		err_cons->printf("Joining threads\n");
		for (set<LoximSession *>::iterator i = open_sessions.begin(); i != open_sessions.end(); i++){
			pthread_join((*i)->get_thread(), NULL);
			delete *i;
		}
		open_sessions.clear();
		err_cons->printf("Threads joined\n");
		return 0;
	}

	void LoximServer::shutdown()
	{
		err_cons->printf("Server shutdown called\n");
		pthread_kill(thread, SIGUSR1);
	}

	void LoximServer::handle_signal(int sig)
	{
		shutting_down = 1;
	}

	/**
	 * It should take care of destroying the thread, the session or
	 * leave it to the shutdown thread.
	 */
	void LoximServer::end_session(LoximSession *session, int code)
	{
		int session_nr = session->get_id();
		pthread_mutex_lock(&open_sessions_mutex);
		if (!shutting_down){
			pthread_detach(session->get_thread());
			open_sessions.erase(session);
			delete session;
		}	
		pthread_mutex_unlock(&open_sessions_mutex);
		err_cons->printf("Session %d ended with code %d, working sessions left: %d\n", session_nr, code, get_sessions_count());
	}

	void LSrv_signal_handler(pthread_t thread, int sig, void *arg)
	{
		LoximServer *serv = (LoximServer*)arg;
		serv->handle_signal(sig);
	}

	int LoximServer::get_sessions_count()
	{
		return open_sessions.size();
	}

	
	int LoximServer::get_config_accept_interval()
	{
		return config_accept_interval;
	}

	int LoximServer::get_config_read_interval()
	{
		return config_read_interval;
	}

	int LoximServer::get_config_max_sessions()
	{
		return config_max_sessions;
	}

	int LoximServer::get_config_max_package_size()
	{
		return config_max_package_size;
	}

	int LoximServer::get_config_keep_alive_interval()
	{
		return config_keep_alive_interval;
	}

	bool LoximServer::is_config_auth_trust_allowed()
	{
		return config_auth_trust_allowed;
	}
	
}

