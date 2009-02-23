#ifndef LOXIMSESSION_H
#define LOXIMSESSION_H


#include <string>
#include <Server/Session.h>
#include <QueryParser/QueryParser.h>
#include <QueryExecutor/QueryExecutor.h>
#include <QueryExecutor/QueryResult.h>
#include <SystemStats/SessionStats.h>
#include <Errors/ErrorConsole.h>
#include <Protocol/Packages/Package.h>
#include <Protocol/Streams/PackageStream.h>
#include <Server/Server.h>
#include <Server/Clipboard.h>
#include <Util/smartptr.h>
#include <Util/Concurrency.h>


namespace Server{
	class Server;
	class Worker;
	class KeepAliveThread;
	class Session;

	class UserData {
		protected:
			std::string login;
			std::string passwd;
		public:
			UserData(const std::string &login, const std::string &passwd);
			virtual ~UserData();

			virtual std::string to_string() const;
			virtual const std::string &get_login() const;
			virtual const std::string &get_passwd() const;
	};

	void LS_signal_handler(pthread_t, int, void*);

	class Worker{
		friend void *W_starter(void *arg);
		protected:
			Session &session;
			Errors::ErrorConsole &err_cons;
			int shutting_down;
			pthread_t thread;
			Util::Mutex mutex;
			Util::CondVar idle_cond, completion_cond;
			_smptr::shared_ptr<Protocol::Package> cur_package;
			bool aborting;

			void start_continue();
			void process_package(_smptr::shared_ptr<Protocol::Package> &package);
			void cancel_job(bool synchrounous);
			void cancel_job_locked(bool synchronous, Util::Locker &l);
		public:

			Worker(Session &session);
			void start();

			/**
			 * always synchronous, kills the thread
			 */
			void stop(); //nothrow

			/**
			 * Will throw an exception if the package shouldn't have
			 * arrived. 
			 */
			void submit(std::auto_ptr<Protocol::Package> package); 
	};
	
	
	class KeepAliveThread{
		private:
			friend void *thread_starter(void*);
			Session &session;
			Errors::ErrorConsole &err_cons;
			pthread_t thread;
			bool running;
			bool shutting_down;
			Util::CondVar cond;
			Util::Mutex cond_mutex;
			void main_loop();
			bool answer_received;
		public:
			KeepAliveThread(Session &session);

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
			Session(Server &server, std::auto_ptr<Protocol::PackageStream> stream);
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
			void set_user_data(std::auto_ptr<UserData> &user_data);
			
			/**
			 * Start the session.
			 */
			void start();

			void shutdown(int reason);
		private:
			/* Static members */

			static uint64_t next_id;
			static uint64_t get_new_id();
			
			/******************/
			
			Server &server;
			std::auto_ptr<Protocol::PackageStream> stream;
			Errors::ErrorConsole &err_cons;
			uint64_t id;
			bool shutting_down;
			int error;
			QExecutor::QueryExecutor qEx;
			KeepAliveThread KAthread;
			Worker worker;
			std::auto_ptr<UserData> user_data;
			QParser::QueryParser qPa;
			Clipboard clipboard;
			string sessionid;
			SystemStatsLib::SessionStats stats;
			char salt[20];
			Util::Mutex send_mutex;
			pthread_t thread;
			sigset_t mask;	

			void main_loop();
			/*
			 * res should be const actually, but QueryResult has to
			 * be corrected first
			 */
			std::auto_ptr<Protocol::Package> serialize_res(QExecutor::QueryResult &res) const;
			void respond(std::auto_ptr<Protocol::Package> qres);
			void send_bye();
			void send_ping();
			void send_pong();
			void send_error(int error, const std::string &descr);
			void send_error(int error);
			
			bool init_phase();
			bool authorize(const std::string &login, const std::string &passwd);
			void free_state();
			std::auto_ptr<QExecutor::QueryResult> execute_statement(const std::string &stmt);
		
			void handle_signal(int);
			
			bool is_white(char c) const;
	};
	
	
	void *W_starter(void *arg);

}


#endif
