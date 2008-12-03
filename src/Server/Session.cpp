#include <iostream>
#include <pthread.h>
#include <math.h>
#include <sstream>
#include <signal.h>
#include <AdminParser/AdminExecutor.h>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <SystemStats/AllStats.h>
#include <SystemStats/SessionStats.h>
#include <Server/SignalRouter.h>
#include <Server/Session.h>
#include <Util/Locker.h>
#include <protocol/constants.h>
#include <protocol/auth/auth.h>
#include <protocol/ptools/Package.h>
#include <protocol/packages/WCHelloPackage.h>
#include <protocol/packages/WSHelloPackage.h>
#include <protocol/packages/WCLoginPackage.h>
#include <protocol/packages/WCPasswordPackage.h>
#include <protocol/packages/ASCOkPackage.h>
#include <protocol/packages/WSAuthorizedPackage.h>
#include <protocol/packages/VSCSendValuesPackage.h>
#include <protocol/packages/VSCSendValuePackage.h>
#include <protocol/packages/VSCFinishedPackage.h>
#include <protocol/packages/VSCAbortPackage.h>
#include <protocol/packages/QCStatementPackage.h>
#include <protocol/packages/ASCSetOptPackage.h>
#include <protocol/packages/ASCErrorPackage.h>
#include <protocol/packages/ASCPingPackage.h>
#include <protocol/packages/ASCByePackage.h>
#include <protocol/packages/ASCPongPackage.h>
#include <protocol/packages/QCExecutePackage.h>
#include <protocol/packages/QSExecutingPackage.h>
#include <protocol/packages/data/BagData.h>
#include <protocol/packages/data/SequenceData.h>
#include <protocol/packages/data/ReferenceData.h>
#include <protocol/packages/data/VarcharData.h>
#include <protocol/packages/data/VOIDData.h>
#include <protocol/packages/data/Sint32Data.h>
#include <protocol/packages/data/DoubleData.h>
#include <protocol/packages/data/BoolData.h>
#include <protocol/packages/data/BindingData.h>
#include <protocol/packages/data/DataPart.h>
#include <protocol/packages/data/StructData.h>

using namespace std;
using namespace SystemStatsLib;
using namespace Util;

namespace Server{


	ProtocolLayerWrap::ProtocolLayerWrap(ProtocolLayer0 &layer0) : layer0(layer0)
	{
	}

	auto_ptr<Package> ProtocolLayerWrap::read_package(int type)
	{
		auto_ptr<Package> package(layer0.readPackage());
		if (!package.get())
			throw LoximException(EReceive);
		if (type && package->getPackageType() != type)
			throw LoximException(EProtocol);
		return package;
	}

	auto_ptr<Package> ProtocolLayerWrap::read_package(sigset_t &sig, int *cancel)
	{
		auto_ptr<Package> package(layer0.readPackage(&sig, cancel));
		if (!package.get())
			throw LoximException(EReceive);
		return package;
	}

	void ProtocolLayerWrap::write_package(const Package &package)
	{
		if (!layer0.writePackage(const_cast<Package*>(&package)))
			throw LoximException(ESend);
	}


	void *thread_start_continue(void *arg)
	{
		((Session*)arg)->main_loop();
		return 0;
	}

	void LS_signal_handler(pthread_t thread, int sig, void* arg)
	{
		((Session*)arg)->handle_signal(sig);
	}


	Session::Session(Server &server, auto_ptr<AbstractSocket>
			&socket, ErrorConsole &err_cons) :
		server(server), socket(socket), err_cons(err_cons),
		id(get_new_id()), shutting_down(false), error(0), qEx(this),
		KAthread(*this, err_cons), worker(*this,
		err_cons), bare_layer0(this->socket.get()), layer0(bare_layer0)
	{
		pthread_mutex_init(&send_mutex, 0);
		for (int i = 0; i < 20; i++)
			salt[i] = (char)(rand()%256-128);
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
			err_cons.printf("session %d bootstrapped. active sessions: %d\n", 
					get_id(), server.get_sessions_count());
			if (init_phase()){
				KAthread.start();
				worker.start();
				free_state();
				worker.stop();
				KAthread.stop();
			}
			err_cons.printf("session %d quitting\n", id);
			server.end_session(get_id(), 0);
		} catch (LoximException &ex){
			err_cons << "Caught LoximException in Session::main_loop\n";
			err_cons << ex.to_string();
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


	/**
	 * Pretty ugly, isn't it? ;) Will be rewritten when new protocol is
	 * plugged.
	 */
	DataPart *Session::serialize_res(const QueryResult &qr) const
	{
		DataPart **dparts, *dp;
		QueryBinderResult *qbr;
		switch (qr.type()) {
			case QueryResult::QBAG:
				dparts = new DataPart*[qr.size()];
				for (unsigned int i = 0; i < qr.size(); i++){
					QueryResult *iQ;
					((QueryBagResult*)&qr)->at(i, iQ);
					dparts[i] = serialize_res(*iQ);
				}
				return new BagData(qr.size(), dparts);
			case QueryResult::QSEQUENCE:
				dparts = new DataPart*[qr.size()];
				for (unsigned int i = 0; i < qr.size(); i++){
					QueryResult *iQ;
					((QuerySequenceResult*)&qr)->at(i, iQ);
					dparts[i] = serialize_res(*iQ);
				}
				return new SequenceData(qr.size(), dparts);
			case QueryResult::QSTRUCT:
				dparts = new DataPart*[qr.size()];
				for (unsigned int i = 0; i < qr.size(); i++){
					QueryResult *iQ;
					((QueryStructResult*)&qr)->at(i, iQ);
					dparts[i] = serialize_res(*iQ);
				}
				return new StructData(qr.size(), dparts);
			case QueryResult::QREFERENCE:
				return new
					ReferenceData(
							((QueryReferenceResult*)&qr)->getValue()->toInteger());
			case QueryResult::QSTRING:
				return new VarcharData(
						strdup(((QueryStringResult *)&qr)->getValue().c_str()));
			case QueryResult::QRESULT:
				return 0; //O co chodzi???!!!
			case QueryResult::QNOTHING:
				return new VOIDData();
			case QueryResult::QINT:
				return new Sint32Data(((QueryIntResult *)&qr)->getValue());
			case QueryResult::QDOUBLE:
				return new DoubleData(((QueryDoubleResult*)&qr)->getValue());
			case QueryResult::QBOOL:
				return new BoolData(((QueryBoolResult*)&qr)->getValue());
			case QueryResult::QBINDER:
				qbr = (QueryBinderResult *) &qr;
				dp = serialize_res(*qbr->getItem());
				return new BindingData(new
						CharArray(strdup((qbr->getName().c_str()))),
						dp->getDataType(), dp);
			default:
				err_cons.printf("Unknown type");
					return 0; //TODO ERROR
				break;
		}
		return 0;
	}


	void Session::respond(auto_ptr<DataPart> &qres)
	{
		Locker l(send_mutex);
		layer0.write_package(VSCSendValuesPackage());
		err_cons.printf("SendValues sent\n");
		layer0.write_package(VSCSendValuePackage(0, 0, qres.get())); 
		err_cons.printf("Sent query result\n");
		layer0.write_package(VSCFinishedPackage());
		err_cons.printf("Sending values finished\n");
	}

	void Session::send_bye()
	{
		Locker l(send_mutex);
		layer0.write_package(ASCByePackage("bye from server"));
	}

	void Session::send_ping()
	{
		Locker l(send_mutex);
		layer0.write_package(ASCPingPackage());
	}

	void Session::send_error(int error, const string &descr)
	{
		Locker l(send_mutex);
		
		layer0.write_package(ASCErrorPackage(error, 0, true, new
					CharArray(strdup(descr.c_str())), 0,
					0));
	}

	void Session::send_error(int error)
	{
		return send_error(error, Errors::SBQLstrerror(error));
	}

	bool Session::init_phase()
	{
		layer0.read_package(ID_WCHelloPackage);
		err_cons.printf("Got WCHELLO\n");
		layer0.write_package(WSHelloPackage(PROTO_VERSION_MAJOR, PROTO_VERSION_MINOR,
				LOXIM_VERSION_MAJOR, LOXIM_VERSION_MINOR,
				get_server().get_config_max_package_size(), 0,
				AUTH_TRUST | AUTH_PASS_MYSQL, salt));
		err_cons.printf("WSHELLO sent\n");
		layer0.read_package(ID_WCLoginPackage);
		err_cons.printf("Got WCLOGIN\n");
		layer0.write_package(ASCOkPackage());
		err_cons.printf("ASCOK sent\n");

		auto_ptr<Package> package(layer0.read_package(ID_WCPasswordPackage));
		err_cons.printf("Got login: %s\n",
				((WCPasswordPackage*)(package.get()))->getLogin()->getBuffor());
		if (authorize(((WCPasswordPackage*)(package.get()))->getLogin()->getBuffor(),
					((WCPasswordPackage*)(package.get()))->getPassword()->getBuffor())){
			layer0.write_package(WSAuthorizedPackage());
			err_cons.printf("WSAUTHORIZED sent\n");
			return true;
		} else {
			send_error(EUserUnknown);
			return false;
		}

	}

	bool Session::authorize(const string &login, const string &passwd)
	{
		auto_ptr<QueryResult> qres;
		int res;
		bool correct;
		try{
			execute_statement("begin");
			qres = execute_statement("validate " + login + " " + passwd);
			//do the check
			if (qres->type() != QueryResult::QBOOL)
				return false;
			else
				correct = ((QueryBoolResult*)qres.get())->getValue();
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

		err_cons.printf("In free state :)\n");
		sigset_t sigmask;
		pthread_sigmask(0, NULL, &sigmask);
		sigaddset(&sigmask, SIGUSR1);
		while (!shutting_down){
			auto_ptr<Package> package = layer0.read_package(sigmask, &shutting_down);
			KAthread.set_answered();
			worker.submit(package);
		}
		if (!error){
			send_bye();
			return;
		}
		else
			throw LoximException(error);
		
	}

	bool is_admin_stmt(const string &stmt)
	{
		int i;
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
			err_cons.printf("Executing administrative statement: %s\n", stmt.c_str());
			int error = AdminParser::AdminExecutor::get_instance()->execute(stmt, this);
			if (error) {
				AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
				throw LoximException(error);
			} else{
				AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
				return auto_ptr<QueryResult>(new QueryNothingResult());
			}
		}else{
			err_cons.printf("EXECUTING REGULAR STATEMENT:\n%s\n\n", stmt.c_str());
			TreeNode *tn;
			string tcrs;
			int tcr = qPa.parseIt(id, stmt, tn, tcrs, true, true);
			if (tcr){
				err_cons.printf("Query not parsed\n");
				AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
				throw LoximException(EParse);
			}
			QueryResult *qres;
			int res = qEx.executeQuery(tn, &qres);
			delete tn;
			if (res){
				err_cons.printf("Execute failed\n");
				AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
				throw LoximException(res);
			}
			err_cons.printf("Executed\n");
			err_cons.printf("%s\n", qres->toString().c_str());
			AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
			return auto_ptr<QueryResult>(qres);
		}
	}


	void Session::handle_signal(int i)
	{
		printf("Signal handler :)\n");
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

	Worker::Worker(Session &session, ErrorConsole &err_cons) :
		session(session), err_cons(err_cons), shutting_down(0),
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
			} catch (LoximException &ex) {
				session.shutdown(ex.get_error());
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
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&idle_cond);
		pthread_mutex_unlock(&mutex);
		pthread_join(thread, NULL);
		thread = 0;
	}

	/*
	 * This can actually be tweaked as not everything needs to between lock
	 * and unlock
	 */
	void Worker::submit(auto_ptr<Package> &package)
	{
		Locker l(mutex);
		aborting = false;
		if (package->getPackageType() == ID_ASCPongPackage){
			return;
		}

		if (package->getPackageType() == ID_VSCAbortPackage){
			if (cur_package.get()){
				aborting = true;
				cancel_job(true, true);
				session.qEx.contExecuting();
			}
			cur_package.reset();
			/* 
			 * ignore aborts when they are inappropriate, because
			 * thay may be simply late
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
		if (package->getPackageType() == ID_ASCByePackage){
			err_cons.printf("Client closed connection\n");
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
		switch (package->getPackageType()){
			case ID_VSCSendValuesPackage :
				err_cons.printf("Got VSCSendValues - ignoring\n");
				break;
			case ID_QCStatementPackage:{
				shared_ptr<QCStatementPackage>
					stmt_pkg = dynamic_pointer_cast<QCStatementPackage>(package);
				if (stmt_pkg->getFlags() & STATEMENT_FLAG_EXECUTE){
					err_cons.printf("Got SCStatement - executing\n");
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
							(session.execute_statement(stmt_pkg->getStatement()->getBuffor()));
					} catch (LoximException &ex) {
						if (!aborting)
							session.send_error(ex.get_error());
						return;
					} 
					auto_ptr<DataPart> sres(session.serialize_res(*qres));
					session.respond(sres);
					//a temporary hack due to destruction of
					//the serialized result in packages
					//destructor
					sres.release();
					return;
				} else {
					err_cons.printf("Got SCStatement - ignoring\n");
					return;
				}
				break;
			}
			case ID_ASCSetOptPackage:
				err_cons.printf("SCSetOpt - ignoring\n");
				break;
			case ID_QCExecutePackage:
				err_cons.printf("Got SCExecute - ignoring\n");
				break;
				/* 
				 * there is no Abort, Bye and Pong case but this is on
				 * purpose. To avoid race conditions (there is no 
				 * reply to these), these package types are handled in
				 * the protocol thread
				 */
			default: 
				err_cons.printf("Unexpected package of type %d\n",
						package->getPackageType());
				throw LoximException(EProtocol);
			
		}
	}

	void *thread_starter(void *arg)
	{
		((KeepAliveThread*)arg)->main_loop();
		return NULL;
	}

	KeepAliveThread::KeepAliveThread(Session &session, ErrorConsole &err_cons) :
		session(session), err_cons(err_cons), thread(0)
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
		err_cons.printf("KeepAliveThred::main_loop starts\n");
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
				err_cons.printf("Keepalive check\n");
				if (!answer_received){
					err_cons.printf("Keepalive thread has just discovered client death.\n");
					session.shutdown(EClientLost);
					return;
				} else {
					answer_received = false;
					try{
						session.send_ping();
					} catch (exception e) {
						err_cons.printf("Warning: keepalive thread died (couldn't send ping");
						return;
					}
				}

			}
		} while (!shutting_down && (res == ETIMEDOUT || res == 0));
		if (!shutting_down)
			err_cons.printf("Warning: keepalive thread died\n");
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

