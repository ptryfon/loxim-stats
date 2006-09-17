
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/types.h>
#include <errno.h>

#include "../Server/Listener.h"
#include "../Log/Logs.h"
#include "../QueryParser/QueryParser.h"
#include "../QueryParser/TreeNode.h"
#include "../QueryExecutor/QueryResult.h"
#include "../QueryExecutor/QueryExecutor.h"
#include "../Store/DBStoreManager.h"

#include "../TransactionManager/Transaction.h"
#include "../Errors/ErrorConsole.h"
#include "../Store/DBLogicalID.h"
#include "Server.h"
#include "../Driver/Result.h"
#include "../Errors/Errors.h"
#include "../TCPProto/Package.h"
#include "../TCPProto/Tcp.h"

#define PULSE_CHECKER_ACTIVE
#define PACKAGE_SENDING

typedef char TREE_NODE_TYPE;

using namespace Errors;
using namespace std;
using namespace Logs;
using namespace Store;
using namespace QParser;
using namespace QExecutor;
using namespace TManager;
using namespace Driver;
using namespace TCPProto;

volatile sig_atomic_t signalReceived = 0;
pthread_t pthread_master_id;

void sigHandler(int sig);
void sigHupPC(int sig);

Server::Server(int newSock)
{
    Sock = newSock;
    ec = new ErrorConsole("Server");
}

Server::Server(int newSock, int newPcSock)
{
    Sock = newSock;
    pcSock = newPcSock;
    ec = new ErrorConsole("Server");
}


Server::~Server()
{
	if (ec != NULL)
		delete ec;
}

QueryExecutor *Server::getQEx() {
    return qEx;
}

void Server::Test() {
    ErrorConsole ec("Server_Test");
    ec.printf("Hello test\n");
    return;
}

void Server::cancelPC() {
#ifndef PULSE_CHECKER_ACTIVE
    return;
#endif
    int status;
    ErrorConsole ec("Server_cancelPC");
    /*
    ec.printf("Cancelling PC..");
    pthread_mutex_lock(&mut);
    if ((int)pulseId == 0) {
	ec.printf("Bad id!");
	pthread_mutex_unlock(&mut);
	return;
    }
    int res;
    res = pthread_cancel(pulseId);
    if (res!=0)
	ec.printf("[Server.cancelPC]--> pulseChecker cancellation failed!! (%d)\n", res);
    else {
	running_threads_count--;
    }
    pthread_mutex_unlock(&mut);
    ec.printf("[Server.cancelPC]--> pulseChecker cancelled.. joining\n");
    */
    ec.printf("[Server.cancelPC]--> joining PC thread \n");
    pthread_join(pulseChecker_id, (void **)&status);
    ec.printf("[Server.cancelPC]--> joined PC.. locking mutex\n");
    pthread_mutex_lock(&mut);
    running_threads_count--;
    pthread_mutex_unlock(&mut);
    ec.printf("[Server.cancelPC]--> Mutex unlocked. PC joined with status %d \n", status);
    return;
}

int Server::getSocket() {
	return Sock;
}

int Server::getFdContact() {
	return fdContact;
}

void *clientPulseCheck(void *arg) {
    
    ErrorConsole ec("Server_PulseCheck");
    ec.printf("clientPulseCheck starts\n");
    //pthread_exit((void *)0);
    
    Server srv=*(Server *)arg;
    int res=-2;
    int fdCli;
    int newSock;
    char ghostBuff;
    
    fdCli=srv.getPCSocket();
    Listener::ListenOnSocket(fdCli, &newSock);

    ec.printf("PC-RECEIVESTART on socket = %d\n", newSock);
    //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    res = recv(newSock, &ghostBuff, 1, 0);
    ec.printf("PC-RECEIVEEND with code = %d (this socket=%d, this serv socket=%d)\n", res, newSock, srv.getSocket());
	if (res<1) {
	    //ec.printf("Server.clientPulseCheck]--> Error with select..(%d)  \n", res);
	    perror("recv");
	    ec.printf("PULSECHECK - Client lost - stopExecuting!\n");
	    srv.getQEx()->stopExecuting();
	    //ec.printf("...done\n");
	}
	else if (res>0)
	    ec.printf("Server.clientPulseCheck]--> Desc. change(!) (%d)  \n", res);	
    ec.printf("Closing socket..\n");
    Listener::CloseSocket(newSock);
    ec.printf("PC exiting..\n");
    pthread_exit((void *) res);
}

double Server::htonDouble(double inD) {
    double res;
    unsigned int *in = (unsigned int *)(&inD);
    unsigned int *out = (unsigned int *)(&res);
    out[0]=htonl(in[0]);
    out[1]=htonl(in[1]);
    return res;
}

int  Server::SerializeRec(QueryResult *qr)
{
	QueryResult *qres;
	QueryResult *collItem;
	QueryStringResult *stringRes;
	QueryBagResult *bagRes;
	QueryReferenceResult *refRes;
	QueryIntResult *intRes;
	QueryBoolResult *boolRes;
	QueryDoubleResult *doubleRes;
	QueryBinderResult *bindRes;

	int rT;
	Result::ResultType resType;

	int valSize;
	int bagSize;
	unsigned long bufBagLen;
	unsigned int intVal;
	double doubleVal;
	bool boolVal;
	string strVal;
	int retVal;
	int i;

	resType=(Result::ResultType)qr->type();
	rT=qr->type();

	//*ec << "[Server.Serialize]--> Starting";
	ec->printf("[Server.Serialize]--> Now serializing object of type: %d \n",  rT);

	switch (rT) {
		case QueryResult::QBAG:
			resType=Result::BAG;
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			ec->printf("[Server.Serialize]--> Received BAG of size %lu \n", bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				*ec << "[Server.Serialize]-->QueryResult shows type of BAG and says it is no collection";
				return ErrServer+EBadResult;
				return -1;
			}
			bagRes = (QueryBagResult *)qr;
			serialBuf[0]=(char)resType;
			serialBuf++;
			memcpy((void *)serialBuf, (const void *)&bufBagLen, sizeof(bufBagLen));
			ec->printf("[Server.Serialize]--> Bag header: |(type)=%d|(size)=%lu|\n", (int)serialBufBegin[0], ntohl(*(unsigned long *)serialBuf));
			serialBuf=serialBuf+sizeof(bufBagLen);

			//TODO DEPTH
			for (i=0;i<bagSize;i++) {
				ec->printf("[Server.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				if (retVal!=0) {
				    ec->printf("[Server.Serialize]--> getResult failed with %d, returning.. \n", retVal);
				    return retVal;
				}
				SerializeRec(collItem);
			}
			break;
		case QueryResult::QSEQUENCE:
			resType=Result::SEQUENCE;
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			ec->printf("[Server.Serialize]--> Received SEQUENCE of size %lu \n", bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				*ec << "[Server.Serialize]-->QueryResult shows type of SEQUENCE and says it is no collection";
				return ErrServer+EBadResult;
				return -1;
			}
			bagRes = (QueryBagResult *)qr;
			*ec << "[Server.Serialize]--> Adding sequence header";
			serialBuf[0]=(char)resType;
			serialBuf++;
			memcpy((void *)serialBuf, (const void *)&bufBagLen, sizeof(bufBagLen));
			serialBuf=serialBuf+sizeof(bufBagLen);

			//TODO DEPTH
			for (i=0;i<bagSize;i++) {
				ec->printf("[Server.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				if (retVal!=0) {
				    ec->printf("[Server.Serialize]--> getResult failed with %d, returning.. \n", retVal);
				    return retVal;
				}
				SerializeRec(collItem);
			}

			break;
		case QueryResult::QSTRUCT:
			resType=Result::STRUCT;
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			ec->printf("[Server.Serialize]--> Received STRUCT of size %lu \n", bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				*ec << "[Server.Serialize]-->QueryResult shows type of STRUCT and says it is no collection";
				return ErrServer+EBadResult;
			}
			bagRes = (QueryBagResult *)qr;
			*ec << "[Server.Serialize]--> Adding struct header";
			serialBuf[0]=(char)resType;
			serialBuf++;
			memcpy((void *)serialBuf, (const void *)&bufBagLen, sizeof(bufBagLen));
			serialBuf=serialBuf+sizeof(bufBagLen);

			//TODO DEPTH
			for (i=0;i<bagSize;i++) {
				ec->printf("[Server.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				if (retVal!=0) {
				    ec->printf("[Server.Serialize]--> getResult failed with %d, returning.. \n", retVal);
				    return retVal;
				}
				SerializeRec(collItem);
			}

			break;
		case QueryResult::QREFERENCE:
		    *ec << "[Server.Serialize]--> Getting reference";
		    resType=Result::REFERENCE;
		    serialBuf[0]=(char)resType;
		    refRes = (QueryReferenceResult *)qr;
		    strVal=(refRes->getValue())->toString();
		    serialBuf++;
		    valSize=strVal.length();
		    ec->printf("[Server.Serialize]--> Reference value: strVal=%s \n", strVal.c_str());
		    strcpy(serialBuf, strVal.c_str());
		    ec->printf("[Server.Serialize]--> Reference string serialized to: %s \n", serialBuf);
		    serialBuf=serialBuf+valSize+1;
		    break;
		case QueryResult::QSTRING:
		    *ec << "[Server.Serialize]--> Getting STRING";
		    resType=Result::STRING;
		    stringRes=(QueryStringResult *)qr;
		    strVal=stringRes->getValue();
		    valSize=strVal.length();
		    ec->printf("[Server.Serialize]--> Adding string header, string size=%d \n", valSize);
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    strcpy(serialBuf, strVal.c_str());
		    ec->printf("[Server.Serialize]--> String serialized to: %s \n", serialBuf);
		    serialBuf=serialBuf+valSize+1;
		    break;
		case QueryResult::QRESULT:
		    *ec << "[Server.Serialize]--> Getting RESULT (ERROR!)";
		    return -1; //TODO ERRORCODE
		    break;
		 case QueryResult::QNOTHING:
		    *ec << "[Server.Serialize]--> VOID type, sending type indicator only";
		    resType=Result::VOID;
		    serialBuf[0]=(char)resType;
		    *ec << "[Server.Serialize]--> VOID type written";
		    serialBuf++;
		    break;
		case QueryResult::QINT:
		    *ec << "[Server.Serialize]--> INT type, sending type indicator..";
		    resType=Result::INT;
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    intRes = (QueryIntResult *)qr;
		    intVal = intRes->getValue();
		    ec->printf("[Server.Serialize]--> .. followed by int value (%d) \n", intVal);
		    intVal=htonl(intVal);
		    memcpy((void *)serialBuf, (const void *)&intVal, sizeof(intVal));
		    serialBuf=serialBuf+sizeof(intVal);
		    //*ec << "[Server.Serialize]--> buffer written";
		    break;
		case QueryResult::QDOUBLE:
		    *ec << "[Server.Serialize]--> DOUBLE type";
		    resType=Result::DOUBLE;
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    doubleRes = (QueryDoubleResult *)qr;
		    doubleVal = doubleRes->getValue();
		    ec->printf("[Server.Serialize]--> double value is %lf \n", doubleVal);
		    //*ec << "[Server.Serialize]--> Switching double byte order..";
		    doubleVal = htonDouble(doubleVal);
		    *ec << "[Server.Serialize]--> Splitting and byte order change complete";
		    memcpy((void *)serialBuf, (const void *)&doubleVal, sizeof(doubleVal));
		    serialBuf = serialBuf + sizeof(doubleVal);
		    break;
		case QueryResult::QBOOL:
		    *ec << "[Server.Serialize]--> BOOL type, sending type indicator and value";
		    boolRes=(QueryBoolResult *)qr;
		    boolVal=boolRes->getValue();
		    if (boolVal==false)
		        resType=Result::BOOLFALSE;
		    else if (boolVal==true)
			resType=Result::BOOLTRUE;
		    else {
			ec->printf("[Server.Serialize]-->Bool result from Executor corrupted: |%d| \n", (int)boolVal);
			*ec << "[Server.Serialize]-->Sending FALSE..";
			resType=Result::BOOLFALSE;
		    }
		    serialBuf[0]=(char)resType;
		    boolRes = (QueryBoolResult *)qr;
		    serialBuf++;
		    serialBuf[0]=(char)(boolVal);
		    serialBuf++;
		    ec->printf("[Server.Serialize]--> Sending bool: |type=%d|value=%d|\n", (int)resType, (int)boolVal);
		    break;
		case QueryResult::QBINDER:
		    *ec << "[Server.Serialize]--> BINDER type, sending name(string) and result";
		    resType=Result::BINDER;
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    bindRes=(QueryBinderResult *) qr;
		    strVal=bindRes->getName();
		    qres=bindRes->getItem();
		    strcpy(serialBuf, strVal.c_str());
		    serialBuf=serialBuf+(strVal.length())+1;
		    SerializeRec(qres);
		    break;
		default:
		    *ec << "[Server.Serialize]--> object UNRECOGNIZED !";
		    return -1; //TODO ERROR
		    break;
	}
	return 0;
}


int Server::sendError(int errNo) {    
#ifdef PACKAGE_SENDING
    ErrorPackage *ePack = new ErrorPackage();
    ePack->init();
    ePack->setError(errNo);
    *ec << "[Server.sendError]: sending package\n";
    packageSend(ePack, Sock);	
    ePack->deinit();
    return 0;
#endif
    int errMesLen=16;
    Result::ResultType rT=Result::ERROR;
    char *bB;
    char *b;
    bB=(char *)malloc(errMesLen);
    b=bB;
    b[0]=(char)rT;
    b++;
    errNo=htonl(errNo);
    memcpy((void *)b, (const void *)&errNo, sizeof(errNo));
    Send(*&bB, errMesLen);
    //TODO free(bB);
    return 0;
}


int Server::getPCSocket() {
    return pcSock;
}

int Server::pulseCheckRun() {
#ifndef PULSE_CHECKER_ACTIVE
	return 0;
#endif
	//Create PulseChecker thread
	int res;
	res = pthread_create(&pulseChecker_id, NULL, clientPulseCheck, this);
	ec->printf("[Server.Run]--> Pulse-checker created with code %d\n", res);
	if (res!=0) {
	    ec->printf("\n\n\n[Server.Run]--> Pulse-checker thread creation FAILED with %d \n\n\n",  res);
	    
	    //ec->printf("[Server.Run]--> Server continues to run anyway, but performance could be lowered\n");
	}
	else {
	    ec->printf("[Server.Run]--> Pulse-checker CREATED \n");
	    pthread_mutex_lock(&mut);
	    running_threads_count++;
	    pthread_mutex_unlock(&mut);
	}
	return res;
}

int Server::Run()
{
	int size=MAX_MESSG;
	int res=0;

	//Non-critical error occured?
	int ncError=0;

	sigset_t block_cc;

	*ec << "[Server.Run]--> Starts";

	//Signal Handling
	sigemptyset(&block_cc);
	sigaddset(&block_cc, SIGINT);

	sigprocmask(SIG_BLOCK, &block_cc, NULL);

	qPa = new QueryParser();
	qEx = new QueryExecutor();

	qEx->set_priviliged_mode( is_priviliged_mode() );
	TreeNode             *tNode;
	map<string, Result*>  qParams;
	QueryResult          *qResult;

	*ec << "[Server.Run]--> Creating message buffers.. \n";

	serialBufBegin=(char *)malloc(MAX_MESSG);
	serialBufEnd=serialBufBegin+MAX_MESSG;
	serialBufSize=MAX_MESSG;
	serialBuf=serialBufBegin;
	pulseChecker_id=0;
	pulseCheckerIndex=0;

#ifdef PULSE_CHECKER_ACTIVE
	res=pulseCheckRun();
#endif

while (!signalReceived) {

	if (ncError==1)
	    *ec << "[Server.Run]-> a non-critical error occured during the previous session";
    	memset(serialBufBegin, '\0', MAX_MESSG);
	serialBuf=serialBufBegin;
	messgBuff=(char*) malloc(MAX_MESSG);

	ncError=0;

	*ec << "[Server.Run]--> Blocking SIGINT for now..";
	sigprocmask(SIG_BLOCK, &block_cc, NULL);

	//*ec << "[Server.Run]--> Cleaning buffers";

	//Get string from client
	*ec << "[Server.Run]--> Unblocking sigint and receiving query from client";
	sigprocmask(SIG_UNBLOCK, &block_cc, NULL);

	 //res = (Receive(&messgBuff, &size));
	Package* package;
	res = packageReceive(&package, Sock);

	if (res != 0) {
	    ec->printf("[Server.Run]--> Receive returned error code %d (socket %d)\n", res, Sock);
	    cancelPC();
    	    return res;
	} else {
	    ec->printf("[Server.Run]--> Received package type: %d\n", package->getType());
	}


	Package::packageType pType = package->getType(); 

	if (pType == Package::REMOTEQUERY) {
		*ec << "odebralem zadanie remote"; 
		RemoteNode* rNode = new RemoteNode();
		rNode->setLogicalID(((RemoteQueryPackage*)package)->getLogicalID());
		rNode->setDeref(((RemoteQueryPackage*)package)->isDeref());
		tNode = rNode;
		
		*ec << "[Server.Run]--> Blocking sigint again..";
		sigprocmask(SIG_BLOCK, &block_cc, NULL);
		
		ec->printf("[Server.Run]--> remote query 2x\n");
	}

	if ((pType == Package::SIMPLEQUERY) || 
		(pType == Package::PARAMQUERY)) {
			
		SimpleQueryPackage* sqp = (SimpleQueryPackage*) package;
		messgBuff = sqp->getQuery();
		size = sqp->getQuerySize();
		//delete package; // it will be used later as a ParamQuery
	
		if (messgBuff==NULL) {
		    *ec << "[Server.Run]--> Error in receive, client terminated??";
		    *ec << "[Server.Run]--> Assuming client loss for that thread - TERMINATING thread";
		    qEx->abort();
		    qPa->QueryParser::~QueryParser();
		    qEx->QueryExecutor::~QueryExecutor();
		    cancelPC();
		    return ErrServer + EClientLost; //TODO Error
		}
	
	
		*ec << "[Server.Run]--> Blocking sigint again..";
		sigprocmask(SIG_BLOCK, &block_cc, NULL);
	
		ec->printf("[Server.Run]--> Requesting PARSE: |%s| \n", messgBuff);
		res = (qPa->parseIt((string) messgBuff, tNode));
		if (res != 0) {
		    ec->printf("[Server.Run]--> Parser returned error code %d\n", res);
		    sendError(res);
		    ncError=1;
		    continue;
		}
		free(messgBuff);
		*ec << "[Server.Run]--> Query has been parsed ";
	}
	
	if (pType == Package::PARAMQUERY) {
		*ec << "[Server.Run]--> Sending statement";
		StatementPackage* stmtPkg = new StatementPackage();
		stmtPkg->setStmtNr(parsedStatements.size());
		parsedStatements.push_back(tNode);

		res = packageSend(stmtPkg, getSocket());
		if (res != 0) {
			ec->printf("[Server.Run]--> packageSend returned error code %d\n", res);
			*ec << "[SERVER]--> ends with ERROR";
		}
		delete package;
		continue;
	}

	if (pType == Package::PARAMSTATEMENT) {
	  *ec << "[Server.Run]--> Taking statement's number";
	  ParamStatementPackage* psp = dynamic_cast<ParamStatementPackage*> (package);
	  ec->printf("[Server.Run]--> taking parsed statement of nr: %d\n", psp->getStmtNr());
	  tNode   = parsedStatements.at(psp->getStmtNr());
	  //qParams = psp->getParams();
	}

	ec->printf("[Server.Run]--> Requesting EXECUTE on tree node: |%d| \n", (int) tNode);
	if (package->getType() == Package::PARAMSTATEMENT) {
		ParamStatementPackage* psp = (ParamStatementPackage*) package;
		
		*ec <<  "Server: [Server.Run]--> with params ";
		*ec << psp->toString();

		res = (qEx->executeQuery(tNode, &(psp->getQueryParams()), &qResult));
	}
	else {
		res = (qEx->executeQuery(tNode, &qResult));	
	}
	if (res != 0) {
	    ec->printf("[Server.Run]--> Executor returned error code %d\n", res);
	    sendError(res);
	    ncError=1;
	    *ec << "[SERVER]--> ends with ERROR";
	    continue;
	}

	*ec << "[Server.Run]--> EXECUTE complete, checking results..";
	ec->printf("[Server.Run]--> Got qResult=|%d| of size: qResult->size()=|%d| :\n",  (int)qResult, qResult->size());

	if (qResult == 0) {//TODO
	    *ec << "[Server.Run]--> qResult is 0";
	    cancelPC();	
	    return 0;
	}

	*ec << "[Server.Run]--> *****SERIALIZE starts*****";

#ifndef PACKAGE_SENDING
	
	res = (SerializeRec(qResult));
	if (res != 0) {
	    ec->printf("[Server.Run--> Serialize returned error code %d\n", res);
	    sendError(res);
	    *ec << "[SERVER]--> ends with ERROR";
	    cancelPC();
	    return ErrServer+ESerialize;
	}
	
#endif
	*ec << "[Server.Run]--> *****SERIALIZE complete*****";

#ifdef PACKAGE_SENDING	
	SimpleResultPackage *pack = new SimpleResultPackage();
	pack->setQueryResult(qResult);
	pack->prepareBuffers();
	res = packageSend(pack, Sock);
	pack->freeBuffers();
	if (res != 0) {
	    ec->printf("[Server.Run]--> PackageSend returned error code %d\n", res);
	    cancelPC();
	    return ErrServer+ESend;
	}
	delete pack;
#endif

#ifndef PACKAGE_SENDING	
	ec->printf("[Server.Run]--> Sending results to client.. Result type=|%d|\n", (int)serialBufBegin[0]);
	res=(Send(&*serialBufBegin, serialBufSize));
	if (res != 0) {
	    ec->printf("[Server.Run]--> Send returned error code %d\n", res);
	    *ec << "[SERVER]--> ends with ERROR";
	    cancelPC();
	    return ErrServer+ESend;
	}
#endif
}
	*ec << "[Server.Run]--> Releasing message buffers";
	free(messgBuff);

	//TODO DESTROY ALL OBJECTS
	*ec << "[Server.Run]--> Destroying Objects";
	*ec << "[Server.Run]--> Disconnecting";
	Disconnect();

	*ec << "[Server.Run]--> <><><>End<><><>";
        cancelPC();
	return 0;
}

int Server::SExit(int code) {
    ec->printf("[Server.SExit]--> Server thread freeing buffers, destroying QE, QP and disconnecting - returning code |%d|\n", code);
    free(serialBufBegin);
    //free(messgBuff);
    qPa->QueryParser::~QueryParser();
    qEx->abort();
    qEx->QueryExecutor::~QueryExecutor();
    Disconnect();
    ec->printf("[Server.SExit]--> Server THREAD nr |%d| CLEANUP COMPLETE, returning\n", pthread_self());
    return code;
}

void sigHupPC(int sig) {
    ErrorConsole ec("Server_PulseCheck");
    ec.printf("[sigHupPC]--> pulseChecker received sighup signal\n");
    signal(SIGHUP, sigHupPC);
    return;
}

int Server::pulseCheckerNotify() {
	return 0;
}

bool Server::is_priviliged_mode() {
    struct sockaddr_in peer;
    int peer_len;

    peer_len = sizeof(peer);

    if (getpeername(Sock, (struct sockaddr *) &peer, (socklen_t *) &peer_len) == -1) {
	return false;
    }   
    string ip_address = string(inet_ntoa(peer.sin_addr));
    
    return ip_address == "127.0.0.1";
};

