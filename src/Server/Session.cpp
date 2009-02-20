#include <iostream>
#include <pthread.h>
#include <math.h>
#include <sstream>
#include <signal.h>
#include <Util/Locker.h>
#include <Util/smartptr.h>
#include <AdminParser/AdminExecutor.h>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <SystemStats/AllStats.h>
#include <SystemStats/SessionStats.h>
#include <Server/SignalRouter.h>
#include <Server/Session.h>
#include <Protocol/Enums/Enums.h>
#include <Protocol/Exceptions.h>
#include <Protocol/Streams/PackageStream.h>
#include <Protocol/Packages/ASCByePackage.h>
#include <Protocol/Packages/ASCErrorPackage.h>
#include <Protocol/Packages/ASCOkPackage.h>
#include <Protocol/Packages/ASCPingPackage.h>
#include <Protocol/Packages/ASCPongPackage.h>
#include <Protocol/Packages/ASCSetoptPackage.h>
#include <Protocol/Packages/QCExecutePackage.h>
#include <Protocol/Packages/QCStatementPackage.h>
#include <Protocol/Packages/VSCAbortPackage.h>
#include <Protocol/Packages/VSCFinishedPackage.h>
#include <Protocol/Packages/VSCSendvaluePackage.h>
#include <Protocol/Packages/VSCSendvaluesPackage.h>
#include <Protocol/Packages/WCHelloPackage.h>
#include <Protocol/Packages/WCLoginPackage.h>
#include <Protocol/Packages/WCPasswordPackage.h>
#include <Protocol/Packages/WSAuthorizedPackage.h>
#include <Protocol/Packages/WSHelloPackage.h>
#include <Protocol/Packages/Data/BagPackage.h>
#include <Protocol/Packages/Data/BindingPackage.h>
#include <Protocol/Packages/Data/BoolPackage.h>
#include <Protocol/Packages/Data/DoublePackage.h>
#include <Protocol/Packages/Data/RefPackage.h>
#include <Protocol/Packages/Data/SequencePackage.h>
#include <Protocol/Packages/Data/Sint32Package.h>
#include <Protocol/Packages/Data/StructPackage.h>
#include <Protocol/Packages/Data/VarcharPackage.h>
#include <Protocol/Packages/Data/VoidPackage.h>

using namespace std;
using namespace _smptr;
using namespace SystemStatsLib;
using namespace Util;
using namespace Protocol;
using namespace QExecutor;

namespace Server{

	void *thread_start_continue(void *arg)
	{
		reinterpret_cast<Session*>(arg)->main_loop();
		return 0;
	}

	void LS_signal_handler(pthread_t thread, int sig, void* arg)
	{
		reinterpret_cast<Session*>(arg)->handle_signal(sig);
	}


	Session::Session(Server &server, auto_ptr<PackageStream>
			stream) :
		server(server), stream(stream),
		err_cons(ErrorConsole::get_instance(EC_SERVER)),
		id(get_new_id()), shutting_down(false), error(0), qEx(this),
		KAthread(*this), worker(*this)
	{
		pthread_mutex_init(&send_mutex, 0);
		for (int i = 0; i < 20; i++)
			salt[i] = (char)(rand()%256-128);

		pthread_sigmask(0, NULL, &mask);
		sigaddset(&mask, SIGUSR1);

		stats.setId(this->id);

		/* Utworzenie numeru sesji */
		stringstream ss;
		ss << "SESSION_" << (this->id);
		ss >> sessionid;

		AllStats::getHandle()->getSessionsStats()->addSessionStats(sessionid,
				&stats);
	}

	Session::~Session()
	{
		AllStats::getHandle()->getSessionsStats()->removeSessionStats(sessionid);
		AllStats::getHandle()->getQueriesStats()->endSession(this->id);
	}

	uint64_t Session::get_id() const
	{
		return id;
	}

	Server &Session::get_server() const
	{
		return server;
	}

	pthread_t Session::get_thread() const
	{
		return thread;
	}

	const UserData *Session::get_user_data() const
	{
		return user_data.get();
	}

	void Session::set_user_data(auto_ptr<UserData> &user_data)
	{
		this->user_data = auto_ptr<UserData>(user_data);
	}


	void Session::start()
	{
		SignalRouter::spawn_and_register(&thread, thread_start_continue,
				LS_signal_handler, this, SIGUSR1);
	}

	void Session::main_loop()
	{
		try{
			qEx.set_priviliged_mode(true);
			qEx.initCg();
			info_printf(err_cons, "session %d bootstrapped. active sessions: %d", 
					get_id(), server.get_sessions_count());
			if (init_phase()){
				KAthread.start();
				worker.start();
				free_state();
				worker.stop();
				KAthread.stop();
			}
			info_printf(err_cons, "session %d quitting", id);
			server.end_session(get_id(), 0);
		} catch (LoximException &ex){
			info_print(err_cons, "Caught LoximException in Session::main_loop");
			debug_print(err_cons, ex.to_string());
			worker.stop();
			KAthread.stop();
			server.end_session(get_id(), ex.get_error());
		} catch (...){
			//the following calls are assumed not to throw anything
			worker.stop();
			KAthread.stop();
			server.end_session(get_id(), EUnknown);
		}
		//don't touch any properties from now on - the object may not exist anymore!
	}

	void Session::shutdown(int reason)
	{
		error = reason;
		pthread_kill(thread, SIGUSR1);
	}


	uint64_t Session::next_id = 0;

	uint64_t Session::get_new_id()
	{
		return next_id++;
	}


	auto_ptr<Package> Session::serialize_res(QueryResult &qr) const
	{
		switch (qr.type()) {
			case QueryResult::QBAG:
				{
					auto_ptr<CollectionPackage> res(new BagPackage());
					for (unsigned int i = 0; i < qr.size(); i++){
						QueryResult *iQ;
						dynamic_cast<QueryBagResult&>(qr).at(i, iQ);
						res->push_back(serialize_res(*iQ));
					}
					return auto_ptr<Package>(res);
				}
			case QueryResult::QSEQUENCE:
				{
					auto_ptr<CollectionPackage> res(new SequencePackage());
					for (unsigned int i = 0; i < qr.size(); i++){
						QueryResult *iQ;
						dynamic_cast<QuerySequenceResult&>(qr).at(i, iQ);
						res->push_back(serialize_res(*iQ));
					}
					return auto_ptr<Package>(res);
				}
			case QueryResult::QSTRUCT:
				{
					auto_ptr<CollectionPackage> res(new StructPackage());
					for (unsigned int i = 0; i < qr.size(); i++){
						QueryResult *iQ;
						dynamic_cast<QueryStructResult&>(qr).at(i, iQ);
						res->push_back(serialize_res(*iQ));
					}
					return auto_ptr<Package>(res);
				}
			case QueryResult::QREFERENCE:
				return auto_ptr<Package>(new RefPackage(dynamic_cast<QueryReferenceResult&>(qr).getValue()->toInteger()));
			case QueryResult::QSTRING:
				{
					auto_ptr<ByteBuffer> buffer(new ByteBuffer(dynamic_cast<QueryStringResult&>(qr).getValue()));
					return auto_ptr<Package>(new VarcharPackage(buffer));
				}
			case QueryResult::QRESULT:
				throw runtime_error("unimplemented");
			case QueryResult::QNOTHING:
				return auto_ptr<Package>(new VoidPackage());
			case QueryResult::QINT:
				return auto_ptr<Package>(new Sint32Package(dynamic_cast<QueryIntResult&>(qr).getValue()));
			case QueryResult::QDOUBLE:
				return auto_ptr<Package>(new DoublePackage(dynamic_cast<QueryDoubleResult&>(qr).getValue()));
			case QueryResult::QBOOL:
				return auto_ptr<Package>(new BoolPackage(dynamic_cast<QueryBoolResult&>(qr).getValue()));
			case QueryResult::QBINDER:
				{
					QueryBinderResult &qbr(dynamic_cast<QueryBinderResult&>
							(qr));
					auto_ptr<ByteBuffer> buffer(new ByteBuffer(qbr.getName()));
					return auto_ptr<Package>(new BindingPackage(buffer, serialize_res(*qbr.getItem())));
				}
			default:
				throw runtime_error("unimplemented");
		}
	}


	void Session::respond(auto_ptr<Package> qres)
	{
		Locker l(send_mutex);
		stream->write_package(mask, shutting_down, VSCSendvaluesPackage(VarUint(0, false), VarUint(0, false), VarUint(0, false), VarUint(0, false)));
		debug_print(err_cons, "SendValues sent");
		VSCSendvaluePackage val(VarUint(0, false), 0, qres);
		stream->write_package(mask, shutting_down, val); 
		debug_print(err_cons, "Sent query result");
		stream->write_package(mask, shutting_down, VSCFinishedPackage());
		debug_print(err_cons, "Sending values finished");
	}

	void Session::send_bye()
	{
		Locker l(send_mutex);
		auto_ptr<ByteBuffer> buffer(new ByteBuffer("bye from server"));
		ASCByePackage p(0, buffer);
		stream->write_package(mask, shutting_down, p);
	}

	void Session::send_ping()
	{
		Locker l(send_mutex);
		stream->write_package(mask, shutting_down, ASCPingPackage());
	}
	
	void Session::send_pong()
	{
		Locker l(send_mutex);
		stream->write_package(mask, shutting_down, ASCPongPackage());
	}

	void Session::send_error(int error, const string &descr)
	{
		Locker l(send_mutex);
		auto_ptr<ByteBuffer> b(new ByteBuffer(descr));
		ASCErrorPackage p(error, VarUint(0, false), b, 0, 0);
		stream->write_package(mask, shutting_down, p);
	}

	void Session::send_error(int error)
	{
		return send_error(error, ::Errors::SBQLstrerror(error));
	}

	bool Session::init_phase()
	{
		stream->read_package_expect(mask, shutting_down, W_C_HELLO_PACKAGE);
		debug_print(err_cons, "Got WCHELLO");
		{
			auto_ptr<ByteBuffer> buf(new ByteBuffer(salt));
			WSHelloPackage p(0, 0, 0, 0,
					get_server().get_config_max_package_size(),
					0, AM_TRUST, buf);
			stream->write_package(mask, shutting_down, p);
			debug_print(err_cons, "WSHELLO sent");
		}
		stream->read_package_expect(mask, shutting_down, W_C_LOGIN_PACKAGE);
		debug_print(err_cons, "Got WCLOGIN");
		{
			ASCOkPackage p;
			stream->write_package(mask, shutting_down, p);
			debug_print(err_cons, "ASCOK sent");
		}

		auto_ptr<Package> package(stream->read_package_expect(mask, shutting_down, W_C_PASSWORD_PACKAGE));
		debug_printf(err_cons, "Got login: %s",
				dynamic_cast<WCPasswordPackage&>(*package).get_val_login().to_string().c_str());
		if (authorize(dynamic_cast<WCPasswordPackage&>(*package).get_val_login().to_string(),
					dynamic_cast<WCPasswordPackage&>(*package).get_val_password().to_string())){
			WSAuthorizedPackage ap;
			stream->write_package(mask, shutting_down, ap);
			debug_print(err_cons, "WSAUTHORIZED sent");
			return true;
		} else {
			send_error(EUserUnknown);
			return false;
		}

	}

	bool Session::authorize(const string &login, const string &passwd)
	{
		auto_ptr<QueryResult> qres;
		bool correct;
		try{
			execute_statement("begin");
			qres = execute_statement("validate " + login + " " + passwd);
			//do the check
			if (qres->type() != QueryResult::QBOOL)
				return false;
			else
				correct = (dynamic_cast<QueryBoolResult&>(*qres)).getValue();
			execute_statement("end");
			if (correct) {
				stats.setUserLogin(login);
			}
			return correct;
		} catch (LoximException &ex) {
			return false;
		}
	}

	void Session::free_state()
	{
		debug_print(err_cons, "In free state :)");
		try{
			while (!shutting_down){
				auto_ptr<Package> package = stream->read_package(mask, shutting_down);
				KAthread.set_answered();
				worker.submit(package);
			}
		} catch (OperationCancelled &ex) {
			//ignore this one - this is the way we shut down
		}
		if (!error){
			try {
				send_bye();
			} catch (...) {} //we don't care about errors here
			return;
		}
		else
			throw LoximException(error);
		
	}

	bool is_admin_stmt(const string &stmt)
	{
		size_t i;
		for (i = 0; i < stmt.length() && isspace(stmt.at(i)); ++i) { }
		if (i == stmt.length())
			return false;
		return stmt.at(i) == '#';

	}


	auto_ptr<QueryResult> Session::execute_statement(const string &stmt)
	{
		AllStats::getHandle()->getQueriesStats()->beginExecuteQuery(get_id(), stmt.c_str());

		if (is_admin_stmt(stmt)){
			if (!qEx.is_dba()){
				AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
				throw LoximException(EParse);
			}
			debug_printf(err_cons, "Executing administrative statement: %s", stmt.c_str());
			int error = AdminParser::AdminExecutor::get_instance()->execute(stmt, this);
			if (error) {
				AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
				throw LoximException(error);
			} else{
				AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
				return auto_ptr<QueryResult>(new QueryNothingResult());
			}
		}else{
			debug_printf(err_cons, "EXECUTING REGULAR STATEMENT:\n%s\n", stmt.c_str());
			TreeNode *tn;
			string tcrs;
			int tcr = qPa.parseIt(id, stmt, tn, tcrs, true, true);
			if (tcr){
				debug_print(err_cons, "Didn't parse the query");
				AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
				throw LoximException(EParse);
			}
			QueryResult *qres;
			int res = qEx.executeQuery(tn, &qres);
			delete tn;
			if (res){
				debug_print(err_cons, "Execute failed");
				AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
				throw LoximException(res);
			}
			debug_print(err_cons, "Executed");
			debug_print(err_cons, qres->toString());
			AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
			return auto_ptr<QueryResult>(qres);
		}
	}


	void Session::handle_signal(int i)
	{
		shutting_down = 1;
	}


	UserData::UserData(const string &login, const string &passwd)
	{
		this->login     = login;
		this->passwd    = passwd;
	}

	UserData::~UserData() {}

	const string &UserData::get_login() const
	{
		return login;
	}

	const string &UserData::get_passwd() const
	{
		return passwd;
	}

	string UserData::to_string() const
	{
		string message = "User data object: " + login + ", " + passwd;
		return message;
	}




	void *W_starter(void *arg)
	{
		((Worker*)arg)->start_continue();
		return NULL;
	}

	Worker::Worker(Session &session) :
		session(session),
		err_cons(ErrorConsole::get_instance(EC_SERVER)),
		shutting_down(0),
		thread(0)
	{
		pthread_mutex_init(&mutex, 0);
		pthread_cond_init(&idle_cond, 0);
		pthread_cond_init(&completion_cond, 0);
	}

	void Worker::start()
	{
		pthread_create(&thread, NULL, W_starter, this);
	}


	void Worker::start_continue()
	{
		//main loop
		pthread_mutex_lock(&mutex);
		while (true){
			if (shutting_down){
				pthread_mutex_unlock(&mutex);
				return;
			}
			pthread_cond_wait(&idle_cond, &mutex);
			if (shutting_down){
				pthread_cond_signal(&completion_cond);
				pthread_mutex_unlock(&mutex);
				return;
			}
			pthread_mutex_unlock(&mutex);
			try{
				process_package(cur_package);
			} catch (LoximException &ex) {
				session.shutdown(ex.get_error());
				//we don't own the mutex
				return;
			} catch (...) {
				session.shutdown(EUnknown);
				return;
			}
			pthread_mutex_lock(&mutex);
			cur_package.reset();
			pthread_cond_signal(&completion_cond);
			if (shutting_down){
				pthread_mutex_unlock(&mutex);
				return;
			}
		}
	}

	//TODO It might be a race condition!!!
	void Worker::cancel_job(bool synchronous, bool mutex_locked)
	{
		session.qEx.stopExecuting();
		if (synchronous){
			if (!mutex_locked)
				pthread_mutex_lock(&mutex);
			if (cur_package.get())
				pthread_cond_wait(&completion_cond, &mutex);
			if (!mutex_locked)
				pthread_mutex_unlock(&mutex);
		}
	}

	void Worker::stop()
	{
		if (!thread)
			return;
		shutting_down = 1;
		cancel_job(false, false);
		{
			Locker l(mutex);
			pthread_cond_signal(&idle_cond);
		}
		pthread_join(thread, NULL);
		thread = 0;
	}

	/*
	 * This can actually be tweaked as not everything needs to between lock
	 * and unlock
	 */
	void Worker::submit(auto_ptr<Package> package)
	{
		Locker l(mutex);
		aborting = false;
		if (package->get_type() == A_SC_PING_PACKAGE){
			session.send_pong();
		}
		if (package->get_type() == A_SC_PONG_PACKAGE){
			return;
		}

		if (package->get_type() == V_SC_ABORT_PACKAGE){
			if (cur_package.get()){
				aborting = true;
				cancel_job(true, true);
				session.qEx.contExecuting();
			}
			cur_package.reset();
			/* 
			 * ignore aborts when they are inappropriate, because
			 * they may be simply late
			 */
			return;
		}
		//there is a package being worked on and an inappropriate package
		//is received
		if (cur_package.get()){
			/* no need to wait for it */
			cancel_job(false, true);
			throw LoximException(EProtocol);
		}
		//no package is being worked on
		if (package->get_type() == A_SC_BYE_PACKAGE){
			info_print(err_cons, "Client closed connection");
			session.shutdown(0);
			return;
		} else { 
			//a regular package
			cur_package = shared_ptr<Package>(package);
			pthread_cond_signal(&idle_cond);
			return;
		}
	}


	void Worker::process_package(shared_ptr<Package> &package)
	{
		switch (package->get_type()){
			case V_SC_SENDVALUES_PACKAGE :
				warning_print(err_cons, "Got VSCSendValues - ignoring");
				break;
			case Q_C_STATEMENT_PACKAGE:{
				shared_ptr<QCStatementPackage>
					stmt_pkg = dynamic_pointer_cast<QCStatementPackage>(package);
				if (stmt_pkg->get_val_flags() & SF_EXECUTE){
					debug_print(err_cons, "Got SCStatement - executing");
					auto_ptr<QueryResult> qres;
					/* We've got a very poor error design, so
					 * what we want here is to send every
					 * error to the client, even if it is a
					 * serious one. The only situation when
					 * we want to report an exception to the
					 * caller is when sending the result or
					 * fails.
					 */
					try{
						qres = auto_ptr<QueryResult>
							(session.execute_statement(stmt_pkg->get_val_statement().to_string()));
					} catch (LoximException &ex) {
						if (!aborting)
							session.send_error(ex.get_error());
						return;
					} 
					auto_ptr<Package> sres(session.serialize_res(*qres));
					session.respond(sres);
					return;
				} else {
					warning_print(err_cons, "Got SCStatement - ignoring");
					return;
				}
				break;
			}
			case A_SC_SETOPT_PACKAGE:
				warning_print(err_cons, "SCSetOpt - ignoring");
				break;
			case Q_C_EXECUTE_PACKAGE:
				warning_print(err_cons, "Got SCExecute - ignoring");
				break;
				/* 
				 * there is no Abort, Bye and Pong case but this is on
				 * purpose. To avoid race conditions (there is no 
				 * reply to these), these package types are handled in
				 * the protocol thread
				 */
			default: 
				warning_printf(err_cons, "Unexpected package of type %llu",
						package->get_type());
				throw LoximException(EProtocol);
			
		}
	}

	void *thread_starter(void *arg)
	{
		((KeepAliveThread*)arg)->main_loop();
		return NULL;
	}

	KeepAliveThread::KeepAliveThread(Session &session) :
		session(session),
		err_cons(ErrorConsole::get_instance(EC_SERVER)), thread(0)
	{
		pthread_mutex_init(&(this->cond_mutex), 0);
		pthread_cond_init(&(this->cond), 0);
		this->shutting_down = false;
	}

	int KeepAliveThread::start()
	{
		return pthread_create(&thread, NULL, thread_starter, this);
	}

	void KeepAliveThread::stop()
	{
		if (!thread)
			return;
		shutting_down = 1;
		pthread_cond_signal(&cond);
		pthread_join(thread, NULL);
		thread = 0;
	}

	void KeepAliveThread::main_loop()
	{
		debug_print(err_cons, "KeepAliveThred::main_loop starts");
		struct timeval now;
		struct timespec tout;
		int res;
		pthread_mutex_lock(&cond_mutex);
		answer_received = true;
		do {
			gettimeofday(&now, 0);
			tout.tv_sec = now.tv_sec + session.get_server().get_config_keep_alive_interval();
			tout.tv_nsec = now.tv_usec * 1000;
			res = pthread_cond_timedwait(&cond, &cond_mutex, &tout);
			if (!shutting_down && res == ETIMEDOUT){
				//actual action
				debug_print(err_cons, "Keepalive check");
				if (!answer_received){
					info_print(err_cons, "Keepalive thread has just discovered client death.");
					session.shutdown(EClientLost);
					return;
				} else {
					answer_received = false;
					try{
						session.send_ping();
					} catch (exception e) {
						warning_print(err_cons, "Warning: keepalive thread died (couldn't send ping");
						return;
					}
				}

			}
		} while (!shutting_down && (res == ETIMEDOUT || res == 0));
		if (!shutting_down)
			warning_print(err_cons, "Warning: keepalive thread died");
		else
			res = 0;
		pthread_mutex_unlock(&cond_mutex);
		pthread_exit(NULL);
	}

	void KeepAliveThread::set_answered()
	{
		answer_received = true;
	}

}

