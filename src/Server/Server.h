#ifndef LOXIMSERVER_H_
#define LOXIMSERVER_H_

#include <set>
#include <string>
#include <Server/Session.h>
#include <Protocol/TCPServer.h>
#include <Config/SBQLConfig.h>
#include <Util/smartptr.h>
#include <Util/Concurrency.h>
#include <Util/SignalReceiver.h>

namespace Server{


	class Session;
	class Server : public Util::SignalReceiver{
		private:
			bool shutting_down;
			Errors::ErrorConsole &err_cons;
			std::map<uint64_t, _smptr::shared_ptr<Session> > open_sessions;
			Util::Mutex open_sessions_mutex;
			int config_max_sessions;
			int config_max_package_size;
			int config_keep_alive_interval;
			bool config_auth_trust_allowed;
			Protocol::TCPServer server;
			
			void signal_handler(int sig);
		public:

			Server(uint32_t host, int port, const Config::SBQLConfig &config);
			~Server();

			// server settings specified in the configuration file
			int get_config_max_sessions() const;
			int get_config_max_package_size() const;
			int get_config_keep_alive_interval() const;
			bool is_config_auth_trust_allowed() const;
			
			int get_sessions_count() const;

			void main_loop();
			void shutdown();


			/**
			 * no one should touch it except for the session thread
			 */
			void end_session(uint64_t, int code);
	};
}


#endif /*LOXIMSERVER_H_*/
