#ifndef LOXIMSERVER_H_
#define LOXIMSERVER_H_

#include <set>
#include <string>
#include "LoximSession.h"
#include <protocol/sockets/TCPIPServerSocket.h>
#include <Config/SBQLConfig.h>
#include <pthread.h>

using namespace std;
using namespace protocol;

#define CONFIG_ACCEPT_INTERVAL_NAME "accept_interval"
#define CONFIG_ACCEPT_INTERVAL_DEFAULT 15
#define CONFIG_READ_INTERVAL_NAME "read_interval"
#define CONFIG_READ_INTERVAL_DEFAULT 60
#define CONFIG_MAX_SESSIONS_NAME "max_sessions"
#define CONFIG_MAX_SESSIONS_DEFAULT 5
#define CONFIG_MAX_PACKAGE_SIZE_NAME "package_size"
#define CONFIG_MAX_PACKAGE_SIZE_DEFAULT 131072
#define CONFIG_AUTH_TRUST_ALLOWED_NAME "auth_trust_allowed"
#define CONFIG_AUTH_TRUST_ALLOWED_DEFAULT false
#define CONFIG_KEEP_ALIVE_INTERVAL_NAME "keep_alive_interval"
#define CONFIG_KEEP_ALIVE_INTERVAL_DEFAULT 60

namespace LoximServer{

	void LSrv_signal_handler(pthread_t, int, void*);

	class LoximSession;
	class LoximServer{
		friend void LSrv_signal_handler(pthread_t, int, void*);
		private:
			int config_accept_interval;
			int config_read_interval;
			int config_max_sessions;
			int config_max_package_size;
			int config_keep_alive_interval;
			bool config_auth_trust_allowed;
			pthread_t thread;
			void handle_signal(int sig);
		public:

			// server settings specified in the configuration file
			int get_config_accept_interval();
			int get_config_read_interval();
			int get_config_max_sessions();
			int get_config_max_package_size();
			int get_config_keep_alive_interval();
			bool is_config_auth_trust_allowed();


			LoximServer(string hostname, int port, SBQLConfig *config);
			~LoximServer();

			int prepare();
			int main_loop();
			void shutdown();

			int get_sessions_count();

			/**
			 * no one should touch it except for the session thread
			 */
			void end_session(LoximSession *session, int code);
		protected:
			ErrorConsole *err_cons;
			int prepared, shutting_down;
			set<LoximSession *> open_sessions;
			pthread_mutex_t open_sessions_mutex;
			TCPIPServerSocket *socket;
			string hostname;
			int port;
	};
}


#endif /*LOXIMSERVER_H_*/
