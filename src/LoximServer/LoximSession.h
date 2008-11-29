#ifndef LOXIMSESSION_H
#define LOXIMSESSION_H


#include <string>
#include <LoximServer/LoximSession.h>
#include <QueryParser/QueryParser.h>
#include <QueryExecutor/QueryExecutor.h>
#include <SystemStats/SessionStats.h>
#include <Errors/ErrorConsole.h>
#include <protocol/sockets/AbstractSocket.h>
#include <protocol/layers/ProtocolLayer0.h>
#include <protocol/packages/data/DataPart.h>
#include <LoximServer/LoximServer.h>
#include <LoximServer/Clipboard.h>

using namespace protocol;
using namespace SystemStatsLib;

namespace LoximServer{
	class LoximServer;
	class Worker;
	class KeepAliveThread;
	class LoximSession;

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
			LoximSession &session;
			pthread_cond_t idle_cond, completion_cond;
			auto_ptr<Package> cur_package;
			ErrorConsole &err_cons;

			void start_continue();
			void process_package(auto_ptr<Package> package);
		public:
			enum sub_ret_t {
				SUBM_SUCCESS = 1, /* package submitted */
				SUBM_EXIT,        /* package turned out to be bye, so we should exit */
				SUBM_ERROR        /* the package shouldn't have arrived now */
			};

			Worker(LoximSession &session, ErrorConsole &err_cons);
			void start();
			void cancel_job(bool synchrounous, bool mutex_locked);

			/**
			 * always synchronous, kills the thread
			 */
			void stop();

			/**
			 * 0 - submitted
			 * 1 - work in progress, error
			 */
			int submit(auto_ptr<Package> package);
	};
	
	
	class KeepAliveThread{
		private:
			friend void *thread_starter(void*);
			bool running;
			bool shutting_down;
			pthread_cond_t cond;
			pthread_mutex_t cond_mutex;
			void main_loop();
			LoximSession &session;
			pthread_t thread;
			ErrorConsole &err_cons;
			int error;
			bool answer_received;
		public:
			KeepAliveThread(LoximSession &session, ErrorConsole &err_cons);
			int start();
			int shutdown();
			void set_answered();
	};


	class LoximSession{
		friend class KeepAliveThread;
		friend class Worker;
		friend void LS_signal_handler(pthread_t, int, void*);
		public:
			/** 
			 * the creator should take care to create objects synchronously,
			 * however only LoximServer should do this, so there is no problem
			 */
			LoximSession(LoximServer &server, auto_ptr<AbstractSocket> &socket, ErrorConsole &err_cons);
			~LoximSession();
			
			uint64_t get_id() const;
			LoximServer &get_server() const;
			pthread_t get_thread() const;

			//deprecated and dangerous (overrides  smart pointer)
			const UserData* get_user_data() const;
			
			void set_user_data(auto_ptr<UserData> &user_data);
			
			void start();
			void main_loop();
			void shutdown();
		protected:
			/* Static members */

			static uint64_t next_id;
			static uint64_t get_new_id();
			
			/******************/
			
			uint64_t id;
			auto_ptr<UserData> user_data;
			ErrorConsole &err_cons;
			auto_ptr<AbstractSocket> socket;
			LoximServer &server;
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
		

			DataPart* serialize_res(const QueryResult &res) const;
			void respond(auto_ptr<DataPart> &qres);
			void send_bye();
			void send_error(int error, const string &descr);
			void send_error(int error);
			
			bool init_phase();
			bool authorize(const string &login, const string &passwd);
			int free_state();
			auto_ptr<QueryResult> execute_statement(const string &stmt);
		
			void handle_signal(int);
			
			bool is_white(char c) const;
	};
	
	
	void *W_starter(void *arg);

}


#endif
