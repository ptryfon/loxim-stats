#ifndef LOXIMSESSION_H_
#define LOXIMSESSION_H_


#include <protocol/sockets/AbstractSocket.h>
#include <protocol/layers/ProtocolLayer0.h>
#include <protocol/packages/data/DataPart.h>
#include <QueryParser/QueryParser.h>
#include <QueryExecutor/QueryExecutor.h>
#include "LoximServer.h"
#include <Errors/ErrorConsole.h>
#include "Clipboard.h"
#include "KeepAlive.h"
#include <string>
#include <SystemStats/SessionStats.h>

using namespace protocol;
using namespace SystemStatsLib;

namespace LoximServer{
	class LoximServer;
	class KeepAliveThread;

	class UserData {
		protected:
			string login;
			string passwd;
		public:
			UserData(string login, string passwd);
			virtual ~UserData();

			virtual string to_string();

			/* getters */
			virtual string get_login();
			virtual string get_passwd();
	};


	class LoximSession{
		friend class KeepAliveThread;
		public:
			/** the creator should take care to create objects synchronously,
			 * however only LoximServer should do this, so there is no problem
			 */
			LoximSession(LoximServer *server, AbstractSocket *socket, ErrorConsole *err_cons);
			~LoximSession();
			void start();
			void main_loop();
			pthread_t get_thread();
			void shutdown();
			LoximServer *get_server();
			int get_id();
			void set_user_data(UserData *user_data);
			UserData* get_user_data();
		protected:
			pthread_mutex_t send_mutex;
			UserData *user_data;
			int send_error(int error, const char* descr);
			int send_error(int error);
			char salt[20];
			ErrorConsole *err_cons;
			int id;
			static int next_id;
			static int get_new_id();
			bool shutting_down;
			pthread_t thread;
			void end(int code);
			AbstractSocket *socket;
			LoximServer *server;
			ProtocolLayer0 *layer0;
			QueryParser qPa;
			QueryExecutor *qEx;
			int init_phase();
			int free_state();
			int execute_statement(const char *, QueryResult **res);
			bool is_white(char c);
			Clipboard clipboard;
			bool authorize(const char *login, const char *passwd);
			DataPart *serialize_res(QueryResult *res);
			int respond(DataPart *qres);
			KeepAliveThread *KAthread;
			string sessionid;
			SessionStats* stats;
	};
}

#endif /*LOXIMSESSION_H_*/
