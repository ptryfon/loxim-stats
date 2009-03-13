#include <errno.h>
#include <string>
#include <list>
#include <iostream>
#include <Errors/Exceptions.h>
#include <Protocol/TCPServer.h>
#include <Protocol/Exceptions.h>
#include <Protocol/Streams/PackageCodec.h>
#include <Server/Server.h>
#include <Server/Session.h>
#include <Util/SignalRouter.h>
#include <Util/Concurrency.h>

using namespace std;
using namespace Protocol;
using namespace Util;
using namespace _smptr;
using namespace Errors;

#define CONFIG_MAX_SESSIONS_NAME "max_sessions"
#define CONFIG_MAX_SESSIONS_DEFAULT 5
#define CONFIG_MAX_PACKAGE_SIZE_NAME "package_size"
#define CONFIG_MAX_PACKAGE_SIZE_DEFAULT 131072
#define CONFIG_AUTH_TRUST_ALLOWED_NAME "auth_trust_allowed"
#define CONFIG_AUTH_TRUST_ALLOWED_DEFAULT false
#define CONFIG_KEEP_ALIVE_INTERVAL_NAME "keep_alive_interval"
#define CONFIG_KEEP_ALIVE_INTERVAL_DEFAULT 60

#define DEFAULT_BACKLOG 10

namespace Server{

	Server::Server(uint32_t addr, int port, const SBQLConfig &config) :
		shutting_down(false), err_cons(ErrorConsole::get_instance(EC_SERVER)),
		config_max_sessions(config.getIntDefault(
				CONFIG_MAX_SESSIONS_NAME,
				CONFIG_MAX_SESSIONS_DEFAULT)),
		config_max_package_size(config.getIntDefault(
				CONFIG_MAX_PACKAGE_SIZE_NAME,
				CONFIG_MAX_PACKAGE_SIZE_DEFAULT)),
		config_keep_alive_interval(config.getIntDefault(
				CONFIG_KEEP_ALIVE_INTERVAL_NAME,
				CONFIG_KEEP_ALIVE_INTERVAL_DEFAULT)),
		config_auth_trust_allowed(config.getBoolDefault(
				CONFIG_AUTH_TRUST_ALLOWED_NAME,
				CONFIG_AUTH_TRUST_ALLOWED_DEFAULT)),
		server(addr, port, DEFAULT_BACKLOG)
	{
		SignalRouter::register_thread(thread, *this);
		thread = pthread_self();
	}

	Server::~Server()
	{
	}

	void Server::main_loop()
	{
		sigset_t sigset_tmpl, sigset;
		pthread_sigmask(0, NULL, &sigset_tmpl);
		sigaddset(&sigset_tmpl, SIGUSR1);
		sigset = sigset_tmpl;
		auto_ptr<DataStream> stream;
		try{
			while (!shutting_down){
				auto_ptr<DataStream> stream = server.accept(sigset, shutting_down);
				Locker l(open_sessions_mutex);
				if (!shutting_down && get_sessions_count() < get_config_max_sessions()){
					try{
						auto_ptr<PackageStream> pstream(new PackageCodec(stream, get_config_max_package_size()));
						shared_ptr<Session> new_session(new Session(*this, pstream));
						open_sessions[new_session->get_id()] = new_session;
						new_session->launch();
					} catch (LoximException &ex) {
						warning_print(err_cons, "Caught exception while creating new session:");
						warning_print(err_cons, ex.to_string());
					} catch (...) {
						warning_print(err_cons, "Caught unknown exception while creating new session");
					}
				}
			}
		} catch (OperationCancelled &ex) {
			//this is a normal way of exiting the server
		} catch (LoximException &ex){
			info_print(err_cons, "Caught exception in server loop, shutting down");
			debug_print(err_cons, ex.to_string());
		} catch (...) {
			info_print(err_cons, "Caught unknown exception in server loop, shutting down");
		}
		{
			Locker l(open_sessions_mutex);
			shutting_down = 1;
			info_print(err_cons, "Quitting, telling sessions to shut down");
			for (set<pair<const uint64_t, shared_ptr<Session> > >::iterator i = open_sessions.begin(); i != open_sessions.end(); i++){
				i->second->shutdown(0);
			}
		}
		info_print(err_cons, "Joining threads");
		for (set<pair<const uint64_t, shared_ptr<Session> > >::iterator i = open_sessions.begin(); i != open_sessions.end(); i++){
			pthread_join(i->second->get_thread(), NULL);
		}
		open_sessions.clear();
		info_print(err_cons, "Threads joined");
	}

	void Server::shutdown()
	{
		info_print(err_cons, "Server shutdown called");
		pthread_kill(thread, SIGUSR1);
	}

	void Server::signal_handler(int sig)
	{
		shutting_down = 1;
	}

	/**
	 * It should take care of destroying the thread, the session or
	 * leave it to the shutdown thread.
	 */
	void Server::end_session(uint64_t session_id, int code)
	{
		if (shutting_down)
			return;
		Locker l(open_sessions_mutex);
		if (shutting_down)
			return;

		shared_ptr<Session> session = open_sessions[session_id];
		pthread_detach(session->get_thread());
		open_sessions.erase(session_id);
		info_printf(err_cons, "Session %d ended with code %d (%s), working sessions left: %d", (int)session_id, (int)code, SBQLstrerror(code).c_str(), (int)get_sessions_count());
	}


	int Server::get_sessions_count() const
	{
		return open_sessions.size();
	}

	
	int Server::get_config_max_sessions() const
	{
		return config_max_sessions;
	}

	int Server::get_config_max_package_size() const
	{
		return config_max_package_size;
	}

	int Server::get_config_keep_alive_interval() const
	{
		return config_keep_alive_interval;
	}

	bool Server::is_config_auth_trust_allowed() const
	{
		return config_auth_trust_allowed;
	}
	
}

