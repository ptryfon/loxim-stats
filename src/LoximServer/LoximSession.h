#ifndef LOXIMSESSION_H
#define LOXIMSESSION_H


#include <string>
#include <QueryParser/QueryParser.h>
#include <QueryExecutor/QueryExecutor.h>
#include <SystemStats/SessionStats.h>
#include <Errors/ErrorConsole.h>
#include <protocol/sockets/AbstractSocket.h>
#include <protocol/layers/ProtocolLayer0.h>
#include <protocol/packages/data/DataPart.h>
#include <LoximServer/LoximServer.h>
#include <LoximServer/Clipboard.h>
#include <LoximServer/KeepAlive.h>

using namespace protocol;
using namespace SystemStatsLib;

namespace LoximServer{
	class LoximServer;
	class KeepAliveThread;
	class Worker;

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

	void LS_signal_handler(pthread_t, int, void*);

	class LoximSession{
		friend class KeepAliveThread;
		friend class Worker;
		friend void LS_signal_handler(pthread_t, int, void*);
		public:
			/** 
			 * the creator should take care to create objects synchronously,
			 * however only LoximServer should do this, so there is no problem
			 */
			LoximSession(LoximServer *server, AbstractSocket *socket, ErrorConsole *err_cons);
			~LoximSession();
			
			int get_id();
			LoximServer *get_server();
			pthread_t get_thread();
			UserData* get_user_data();
			
			void set_user_data(UserData *user_data);
			
			void start();
			void main_loop();
			void shutdown();
		protected:
			/* Static members */

			static int next_id;
			static int get_new_id();
			
			/******************/
			
			int id;
			UserData *user_data;
			ErrorConsole *err_cons;
			AbstractSocket *socket;
			LoximServer *server;
			ProtocolLayer0 *layer0;
			QueryParser qPa;
			QueryExecutor *qEx;
			Clipboard clipboard;
			KeepAliveThread *KAthread;
			Worker *worker;
			string sessionid;
			SessionStats* stats;
			char salt[20];
			int error;
			int shutting_down;
			pthread_mutex_t send_mutex;
			pthread_t thread;
		

			DataPart *serialize_res(QueryResult *res);
			int respond(DataPart *qres);
			int send_bye();
			int send_error(int error, const char* descr);
			int send_error(int error);
			
			int init_phase();
			bool authorize(const char *login, const char *passwd);
			int free_state();
			int execute_statement(const char *, QueryResult **res);
		
			void handle_signal(int);
			
			bool is_white(char c);
	};
}

#endif /*LOXIMSESSION_H*/
