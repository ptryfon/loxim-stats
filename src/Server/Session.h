#ifndef LOXIMSESSION_H
#define LOXIMSESSION_H


#include <string>
#include <Server/Session.h>
#include <QueryParser/QueryParser.h>
#include <QueryExecutor/QueryExecutor.h>
#include <SystemStats/SessionStats.h>
#include <Errors/ErrorConsole.h>
#include <protocol/sockets/AbstractSocket.h>
#include <protocol/layers/ProtocolLayer0.h>
#include <protocol/packages/data/DataPart.h>
#include <Server/Server.h>
#include <Server/Clipboard.h>
#include <Util/smartptr.h>

using namespace protocol;
using namespace SystemStatsLib;
using namespace _smptr;

namespace Server{
	class Server;
	class Worker;
	class KeepAliveThread;
	class Session;

	class UserData {
		protected:
			string login;
			string passwd;
		public:
			UserData(const string &login, const string &passwd);
			virtual ~UserData();

			virtual string to_string() const;
			virtual const string &get_login() const;
			virtual const string &get_passwd() const;
	};

	void LS_signal_handler(pthread_t, int, void*);

	/* a temporary solution to introduce exceptions */
	class ProtocolLayerWrap{
		protected:
			ProtocolLayer0 &layer0;
		public:
			ProtocolLayerWrap(ProtocolLayer0 &);
			auto_ptr<Package> read_package(int type = 0);
			auto_ptr<Package> read_package(sigset_t &, int*);
			void write_package(const Package &p);
	};

	class Worker{
		friend void *W_starter(void *arg);
		protected:
			pthread_t thread;
			pthread_mutex_t mutex;
			int shutting_down;
			Session &session;
			pthread_cond_t idle_cond, completion_cond;
			shared_ptr<Package> cur_package;
			ErrorConsole &err_cons;
			bool aborting;

			void start_continue();
			void process_package(shared_ptr<Package> &package);
			void cancel_job(bool synchrounous, bool mutex_locked);
		public:
			enum sub_ret_t {
				SUBM_SUCCESS = 1, /* package submitted */
				SUBM_EXIT,        /* package turned out to be bye, so we should exit */
				SUBM_ERROR        /* the package shouldn't have arrived now */
			};

			Worker(Session &session, ErrorConsole &err_cons);
			void start();

			/**
			 * always synchronous, kills the thread
			 */
			void stop(); //nothrow

			/**
			 * Will throw an exception if the package shouldn't have
			 * arrived. 
			 */
			void submit(auto_ptr<Package> &package); 
	};
	
	
	class KeepAliveThread{
		private:
			friend void *thread_starter(void*);
			bool running;
			bool shutting_down;
			pthread_cond_t cond;
			pthread_mutex_t cond_mutex;
			void main_loop();
			Session &session;
			pthread_t thread;
			ErrorConsole &err_cons;
			bool answer_received;
		public:
			KeepAliveThread(Session &session, ErrorConsole &err_cons);

			/**
			 * Start the thread.
			 */
			int start();

			/**
			 * Kill the thread synchronously. NOTHROW
			 */
			void stop(); 

			/**
			 * Tell the thread, that a package came. NOTHROW
			 */
			void set_answered();
	};


	class Session{
		friend class KeepAliveThread;
		friend class Worker;
		friend void LS_signal_handler(pthread_t, int, void*);
		friend void *thread_start_continue(void*);
		public:
			/** 
			 * The creator should take care to create objects synchronously,
			 * however only Server should do this, so there is no problem.
			 */
			Session(Server &server, auto_ptr<AbstractSocket> &socket, ErrorConsole &err_cons);
			~Session();
			
			uint64_t get_id() const;
			Server &get_server() const;
			pthread_t get_thread() const;

			/**
			 * @deprecated Overrides smart pointer and therefore can
			 * be dangerous.
			 */
			const UserData* get_user_data() const;
			
			/**
			 * Used by the executor
			 */
			void set_user_data(auto_ptr<UserData> &user_data);
			
			/**
			 * Start the session.
			 */
			void start();

			void shutdown(int reason);
		protected:
			/* Static members */

			static uint64_t next_id;
			static uint64_t get_new_id();
			
			/******************/
			
			uint64_t id;
			auto_ptr<UserData> user_data;
			ErrorConsole &err_cons;
			auto_ptr<AbstractSocket> socket;
			Server &server;
			ProtocolLayer0 bare_layer0;
			ProtocolLayerWrap layer0;
			QueryParser qPa;
			QueryExecutor qEx;
			Clipboard clipboard;
			KeepAliveThread KAthread;
			Worker worker;
			string sessionid;
			SessionStats stats;
			char salt[20];
			int error;
			int shutting_down;
			pthread_mutex_t send_mutex;
			pthread_t thread;
		

			void main_loop();
			/**
			 * Ugly, but it requires protocol modifications to be
			 * pretty.
			 */
			DataPart* serialize_res(const QueryResult &res) const;
			void respond(auto_ptr<DataPart> &qres);
			void send_bye();
			void send_ping();
			void send_error(int error, const string &descr);
			void send_error(int error);
			
			bool init_phase();
			bool authorize(const string &login, const string &passwd);
			void free_state();
			auto_ptr<QueryResult> execute_statement(const string &stmt);
		
			void handle_signal(int);
			
			bool is_white(char c) const;
	};
	
	
	void *W_starter(void *arg);

}


#endif
