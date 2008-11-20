#include <iostream>
#include <pthread.h>
#include <math.h>
#include <sstream>
#include <signal.h>
#include <AdminParser/AdminExecutor.h>
#include <Errors/Errors.h>
#include <SystemStats/AllStats.h>
#include <SystemStats/SessionStats.h>
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
#include <Worker.h>
#include <SignalRouter.h>
#include <LoximSession.h>

using namespace std;
using namespace SystemStatsLib;

	#define recieve_or_die(package, layer0, cond) \
	    package = layer0->readPackage(); \
	    if (!package) \
		return EReceive; \
	    if (!(cond)){ \
		delete package; \
		return EProtocol; \
	    }

	#define send_or_die(package, layer0) \
	    if (!layer0->writePackage(package)){ \
		delete package; \
		return ESend; \
	    } else { \
		delete package; \
	    }

static void *thread_start_continue(void *arg)
{
	((LoximServer::LoximSession*)arg)->main_loop();
	return 0;
}

void LoximServer::LS_signal_handler(pthread_t thread, int sig, void* arg)
{
	((LoximSession*)arg)->handle_signal(sig);
}

/*
 * LoximSession
 */

LoximServer::LoximSession::LoximSession(LoximServer *server, AbstractSocket *socket, ErrorConsole *err_cons)
{
	this->server = server;
	this->socket = socket;
	this->layer0 = 0;
	this->err_cons = err_cons;
	this->id = get_new_id();
	this->shutting_down = false;
	this->user_data = NULL;
	this->error = 0;
	this->qEx = new QueryExecutor(this);
	this->KAthread = new KeepAliveThread(this, err_cons);
	this->worker = new Worker(this, err_cons);
	pthread_mutex_init(&send_mutex, 0);
	error = 0;
	for (int i = 0; i < 20; i++)
		salt[i] = (char)(rand()%256-128);
	stats = new SessionStats();
	stats->setId(this->id);

	/* Utworzenie numeru sesji */
	stringstream ss;
	ss << "SESSION_" << (this->id);
	ss >> sessionid;

	AllStats::getHandle()->getSessionsStats()->addSessionStats(sessionid, stats);
}

LoximServer::LoximSession::~LoximSession()
{
	AllStats::getHandle()->getSessionsStats()->removeSessionStats(sessionid);
	AllStats::getHandle()->getQueriesStats()->endSession(this->id);
	delete socket;
	if (user_data)
		delete user_data;
	if (layer0)
		delete layer0;
	delete qEx;
	delete KAthread;
}

int LoximServer::LoximSession::get_id()
{
    return id;
}

LoximServer::LoximServer *LoximServer::LoximSession::get_server()
{
    return server;
}

pthread_t LoximServer::LoximSession::get_thread()
{
	return thread;
}

LoximServer::UserData *LoximServer::LoximSession::get_user_data()
{
	return user_data;
}

void LoximServer::LoximSession::set_user_data(UserData *user_data)
{
	this->user_data = user_data;
}


void LoximServer::LoximSession::start()
{
	//pthread_create(&thread, NULL, thread_start_continue, this);
	SignalRouter::spawn_and_register(&thread, thread_start_continue, LS_signal_handler, this, SIGUSR1);
}

void LoximServer::LoximSession::main_loop()
{
	qEx->set_priviliged_mode(true);
	qEx->initCg();
	err_cons->printf("session %d bootstrapped. active sessions: %d\n", get_id(), server->get_sessions_count());
	layer0 = new ProtocolLayer0(socket);
	int err_code = 0;
	if (!(err_code = init_phase())){
		KAthread->start();
		worker->start();
		err_code = free_state();
		worker->stop();
		KAthread->shutdown();
	}
	err_cons->printf("session %d quitting\n", id);
	socket->close();
	server->end_session(this, err_code);
	//don't touch any properties from now on - the object may not exist anymore!
	pthread_exit(0);
}

void LoximServer::LoximSession::shutdown()
{
	pthread_kill(thread, SIGUSR1);
}


int LoximServer::LoximSession::next_id = 0;

int LoximServer::LoximSession::get_new_id()
{
    return next_id++;
}


DataPart *LoximServer::LoximSession::serialize_res(QueryResult *qr)
{
	DataPart **dparts, *dp;
	QueryBinderResult *qbr;
	switch (qr->type()) {
		case QueryResult::QBAG:
			dparts = new DataPart*[qr->size()];
			for (unsigned int i = 0; i < qr->size(); i++){
				QueryResult *iQ;
				((QueryBagResult*)qr)->at(i, iQ);
				dparts[i] = serialize_res(iQ);
			}
			return new BagData(qr->size(), dparts);
		case QueryResult::QSEQUENCE:
			dparts = new DataPart*[qr->size()];
			for (unsigned int i = 0; i < qr->size(); i++){
				QueryResult *iQ;
				((QuerySequenceResult*)qr)->at(i, iQ);
				dparts[i] = serialize_res(iQ);
			}
			return new SequenceData(qr->size(), dparts);
		case QueryResult::QSTRUCT:
			dparts = new DataPart*[qr->size()];
			for (unsigned int i = 0; i < qr->size(); i++){
				QueryResult *iQ;
				((QueryStructResult*)qr)->at(i, iQ);
				dparts[i] = serialize_res(iQ);
			}
			return new StructData(qr->size(), dparts);
		case QueryResult::QREFERENCE:
			return new ReferenceData(((QueryReferenceResult*)qr)->getValue()->toInteger());
		case QueryResult::QSTRING:
			return new VarcharData(strdup(((QueryStringResult *)qr)->getValue().c_str()));
		case QueryResult::QRESULT:
			return 0; //O co chodzi???!!!
		case QueryResult::QNOTHING:
			return new VOIDData();
		case QueryResult::QINT:
			return new Sint32Data(((QueryIntResult *)qr)->getValue());
		case QueryResult::QDOUBLE:
			return new DoubleData(((QueryDoubleResult*)qr)->getValue());
		case QueryResult::QBOOL:
			return new BoolData(((QueryBoolResult*)qr)->getValue());
		case QueryResult::QBINDER:
			qbr = (QueryBinderResult *) qr;
			dp = serialize_res(qbr->getItem());
			return new BindingData(new CharArray(strdup((qbr->getName().c_str()))), dp->getDataType(), dp);
		default:
			err_cons->printf("Blad serializacji: nieznany typ");
			return 0; //TODO ERROR
			break;
	}
	return 0;
}

int LoximServer::LoximSession::respond(DataPart *qres)
{
	pthread_mutex_lock(&send_mutex);
	Package *package;
	package = new VSCSendValuesPackage();
	send_or_die(package, layer0);
	err_cons->printf("SendValues sent\n");

	package = new VSCSendValuePackage(0, 0, qres);
	send_or_die(package, layer0);
	err_cons->printf("Sent query result\n");

	package = new VSCFinishedPackage();
	send_or_die(package, layer0);
	err_cons->printf("Sending values finished\n");
	pthread_mutex_unlock(&send_mutex);
	return 0;
}

int LoximServer::LoximSession::send_bye()
{
	pthread_mutex_lock(&send_mutex);
	ASCByePackage package("bye from server");
	if (layer0->writePackage(&package)){
		pthread_mutex_unlock(&send_mutex);
		return 0;
	} else {
		pthread_mutex_unlock(&send_mutex);
		return ESend;
	}
}

int LoximServer::LoximSession::send_error(int error, const char *descr)
{
	pthread_mutex_lock(&send_mutex);
	ASCErrorPackage package(error, 0, true, new CharArray(strdup(descr)), 0, 0);
	if (layer0->writePackage(&package)){
		pthread_mutex_unlock(&send_mutex);
		return 0;
	} else {
		pthread_mutex_unlock(&send_mutex);
		return ESend;
	}
}

int LoximServer::LoximSession::send_error(int error)
{
	string *message = Errors::SBQLstrerror(error);
	error = send_error(error, message->c_str());
	delete message;
	return error;
}

int LoximServer::LoximSession::init_phase()
{

	Package *package;

	recieve_or_die(package, layer0, package->getPackageType() == ID_WCHelloPackage);
	delete package;
	err_cons->printf("Got WCHELLO\n");

	package = new WSHelloPackage(PROTO_VERSION_MAJOR, PROTO_VERSION_MINOR, LOXIM_VERSION_MAJOR, LOXIM_VERSION_MINOR, get_server()->get_config_max_package_size(), 0, AUTH_TRUST | AUTH_PASS_MYSQL, salt);
	send_or_die(package, layer0);
	err_cons->printf("WSHELLO sent\n");

	recieve_or_die(package, layer0, package->getPackageType() == ID_WCLoginPackage);
	err_cons->printf("Got WCLOGIN\n");
	delete package;

	package = new ASCOkPackage();
	send_or_die(package, layer0);
	err_cons->printf("ASCOK sent\n");

	recieve_or_die(package, layer0, package->getPackageType() == ID_WCPasswordPackage);
	err_cons->printf("Got login: %s\n", ((WCPasswordPackage*)package)->getLogin()->getBuffor());
	if (authorize(((WCPasswordPackage*)package)->getLogin()->getBuffor(), ((WCPasswordPackage*)package)->getPassword()->getBuffor())){
		delete package;
		package = new WSAuthorizedPackage();
		send_or_die(package, layer0);
		err_cons->printf("WSAUTHORIZED sent\n");
	} else {
		delete package;
		send_error(EUserUnknown);
		return EUserUnknown;
	}
	return 0;

}

bool LoximServer::LoximSession::authorize(const char *login, const char *passwd)
{
	QueryResult *qres;
	int res;
	bool correct;
	res = execute_statement("begin", &qres);
	if (res)
		return false;
	delete qres;
	res = execute_statement(("validate " + string(login) + " " + string(passwd)).c_str(), &qres);
	if (res){
		return false;
	}
	//do the check
	if (qres->type() != QueryResult::QBOOL){
		delete qres;
		return false;
	} else {
		correct = ((QueryBoolResult*)qres)->getValue();
		delete qres;
	}
	res = execute_statement("end", &qres);
	if (res)
		return false;
	delete qres;

	if (correct) {
		stats->setUserLogin(string(login));
	}
	return correct;
}

int LoximServer::LoximSession::free_state()
{

	Package *package;
	err_cons->printf("In free state :)\n");
	sigset_t sigmask, sigmask_tmpl;
	pthread_sigmask(0, NULL, &sigmask_tmpl);
	sigaddset(&sigmask_tmpl, SIGUSR1);
	sigmask = sigmask_tmpl;
	while ((package = layer0->readPackage(&sigmask, &shutting_down)) && !shutting_down){
		sigmask = sigmask_tmpl;
		KAthread->set_answered();
		switch (worker->submit(package)){
			case Worker::SUBM_EXIT:
				shutting_down = 1;
				return 0;
			case Worker::SUBM_ERROR:
				shutting_down = 1;
				return EProtocol;
			case Worker::SUBM_SUCCESS:
				/* don't do anything */
				break;
		}
	}
	if (package)
		delete package;
	if (shutting_down){
		worker->cancel_job(true, false);
		if (!error)
			return send_bye();
		else
			return error;
	}
	return EReceive;

}

int LoximServer::LoximSession::execute_statement(const char *stmt, QueryResult **qres)
{
	AllStats::getHandle()->getQueriesStats()->beginExecuteQuery(get_id(), stmt);

	const char *nw;
	for (nw = stmt; *nw != 0 && is_white(*nw); nw++){};
	if (*nw == '#'){
//		if (!qEx.is_dba()){
//			send_error(EPerm);
//		}
		err_cons->printf("Executing administrative statement: %s\n", stmt);
		int error = AdminParser::AdminExecutor::get_instance()->execute(stmt, this);
		if (error) {
			AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
			return error;
		} else{
			*qres = new QueryNothingResult();
			AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
			return 0;
		}
	}else{
		err_cons->printf("EXECUTING REGULAR STATEMENT:\n%s\n\n", stmt);
		TreeNode *tn;
		string tcrs;
		int tcr = qPa.parseIt(id, stmt, tn, tcrs, true, true);
		if (tcr){
			err_cons->printf("Query not parsed\n");
			AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
			return EParse;
		}
		int res = qEx->executeQuery(tn, qres);
		delete tn;
		if (res){
			err_cons->printf("Execute failed\n");
			AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
			return res;
		}
		err_cons->printf("Executed\n");
		err_cons->printf("%s\n", (*qres)->toString().c_str());
		AllStats::getHandle()->getQueriesStats()->endExecuteQuery(get_id());
		return 0;
	}
}


void LoximServer::LoximSession::handle_signal(int i)
{
	printf("Signal handler :)\n");
	shutting_down = 1;
}


bool LoximServer::LoximSession::is_white(char c)
{
	return c == ' ' || c == '\n' || c == '\t';
}

#undef send_or_die
#undef recieve_or_die

/*
 * UserData
 */


LoximServer::UserData::UserData(string login, string passwd)
{
	this->login     = login;
	this->passwd    = passwd;
}

LoximServer::UserData::~UserData() {}

string LoximServer::UserData::get_login()
{
	return login;
}

string LoximServer::UserData::get_passwd()
{
	return passwd;
}

string LoximServer::UserData::to_string()
{
	string message = "User data object: " + login + ", " + passwd;
	return message;
}

