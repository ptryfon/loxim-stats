
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <setjmp.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>

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

typedef char TREE_NODE_TYPE;


// TODO 
// 1-Inicjalizacja do Listenera
// 2-Watki
// 3-Obsluga bledow
// 4-Lepsza diagnostyka


using namespace Errors;
using namespace std;
using namespace Logs;
using namespace Store;
using namespace QParser;
using namespace QExecutor;
using namespace TManager;
using namespace Driver;

//pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t signalReceived = 0;
pthread_t pthread_master_id;

void sigHandler(int sig);

Server::Server(int newSock)
{
	Sock = newSock;
    ec = new ErrorConsole("Server");
}

Server::~Server()
{
	if (ec != NULL)
		delete ec;
}

double Server::htonDouble(double inD) {
    double res;
    unsigned int *in = (unsigned int *)(&inD);
    unsigned int *out = (unsigned int *)(&res);
    out[0]=htonl(in[0]);
    out[1]=htonl(in[1]);
    return res;
}

//DEVELOPING - NOT YET USED
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
			//*ec << "[Server.Serialize]--> Adding bag header";
			serialBuf[0]=(char)resType;
			//ec->printf("\n SerialBuf[0] |%c|%d| \n\n", serialBuf[0], (int)serialBuf[0]);
			//ec->printf("\n SerialBufBegin[0] |%c|%d| \n\n", serialBufBegin[0], (int)serialBufBegin[0]);
			serialBuf++;
			//ec->printf("\n SerialBuf[0] |%s|%d| \n\n", serialBuf[0], (int)serialBuf[0]);
			//ec->printf("\n SerialBufBegin[0] |%c|%d| \n\n", serialBufBegin[0], (int)serialBufBegin[0]);
			
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
		case QueryResult::QSEQUENCE: //TODO CZYM SIE ROZNI SEQUENCE OD BAGA??
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
			//ec->printf("[Server.Serialize]--> Sequence header: |(type)=%d|(size)=%lu|\n", (int)serialBufBegin[0], ntohl(*(unsigned long *)serialBuf));
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
		case QueryResult::QSTRUCT: //TODO CZYM SIE ROZNI STRUCT OD BAGA??
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
			//ec->printf("[Server.Serialize]--> Struct header: |(type)=%d|(size)=%lu|\n", (int)serialBufBegin[0], ntohl(*(unsigned long *)serialBuf));
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
		    //*ec << "[Server.Serialize]--> String header complete";
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
		case QueryResult::QBOOL: //TODO - pass either BOOLTRUE or BOOLFALSE or just BOOL?
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
		    serialBuf=serialBuf+(strVal.length());
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
    int errMesLen=16;
    Result::ResultType rT=Result::ERROR;
    char *bB;
    char *b;
    bB=(char *)malloc(errMesLen);
    b=bB;
    b[0]=(char)rT;
    b++;
    memcpy((void *)b, (const void *)&errNo, sizeof(errNo));
    Send(*&bB, errMesLen);
    return 0;
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
//	signal(SIGINT, (Server::sigHandler));
	
	//Set process signal mask
	//*ec << "[Server.Run]--> Blocking SIGINT for now...";
	sigprocmask(SIG_BLOCK, &block_cc, NULL);

	*ec << "[Server.Run]--> Initializing objects";
		
	LogManager* logManager = new LogManager();
	logManager->init();
	DBStoreManager* storeManager = new DBStoreManager();
	storeManager->init(logManager);
	TransactionManager::getHandle()->init(storeManager, logManager);
	storeManager->start();
	//storeManager->setTransactionManager(TransactionManager::getHandle());
	
	QueryParser *qPa = new QueryParser();
	QueryExecutor *qEx = new QueryExecutor();
		
	TreeNode *tNode;
	QueryResult *qResult;
	
	*ec << "[Server.Run]--> Creating message buffers.. \n";
	
	messgBuff=(char*) malloc(MAX_MESSG);

	serialBufBegin=(char *)malloc(MAX_MESSG);
	serialBufEnd=serialBufBegin+MAX_MESSG;
	serialBufSize=MAX_MESSG;
	serialBuf=serialBufBegin;
	
	
while (!signalReceived) {

	if (ncError==1)
	    *ec << "[Server.Run]-> a non-critical error occured during the previous session";
    	memset(serialBufBegin, '\0', MAX_MESSG);
	serialBuf=serialBufBegin;	
	ncError=0;

	*ec << "[Server.Run]--> Blocking SIGINT for now..";
	sigprocmask(SIG_BLOCK, &block_cc, NULL);
	
	//*ec << "[Server.Run]--> Cleaning buffers";
	//memset(serializedMessg, '\0', MAX_MESSG); 
	
	//Get string from client
	*ec << "[Server.Run]--> Unblocking sigint and receiving query from client";
	sigprocmask(SIG_UNBLOCK, &block_cc, NULL);
	
	res = (Receive(&messgBuff, &size));
	if (res != 0) {
	    ec->printf("[Server.Run]--> Receive returned error code %d\n", res);
	    return res;
	}    
	if (messgBuff==NULL) {
	    *ec << "[Server.Run]--> Error in receive, client terminated??";
	    *ec << "[Server.Run]--> Assuming client loss for that thread - TERMINATING thread";
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
	    *ec << "[SERVER]--> ends with ERROR";
	    return res;
	}
	
	ec->printf("[Server.Run]--> Requesting EXECUTE on tree node: |%d| \n", (int) tNode);
	res = (qEx->executeQuery(tNode, &qResult)); 
	if (res != 0) {
	    ec->printf("[Server.Run]--> Executor returned error code %d\n", res);
	    sendError(res);
	    ncError=1;
	    continue;
	    *ec << "[SERVER]--> ends with ERROR";
	    return ErrServer+EExecute;
	} 
	
	*ec << "[Server.Run]--> EXECUTE complete, checking results..";
	ec->printf("[Server.Run]--> Got qResult=|%d| of size: qResult->size()=|%d| :\n",  (int)qResult, qResult->size());
	
	if (qResult == 0) {//TODO ERROR
	 return 0;} 
	
	*ec << "[Server.Run]--> *****SERIALIZE starts*****";
	
	res = (SerializeRec(qResult)); 
	if (res != 0) {
	    ec->printf("[Server.Run--> Serialize returned error code %d\n", res);
	    sendError(res);
	    *ec << "[SERVER]--> ends with ERROR";
	    return ErrServer+ESerialize;
	}
	
	*ec << "[Server.Run]--> *****SERIALIZE complete*****";
	
	ec->printf("[Server.Run]--> Sending results to client.. Result type=|%d|\n", (int)serialBufBegin[0]);
	//ec->printf("Buf: |%d|%d|%d|%d|%d|%d|%d|\n", (int)serialBufBegin[0], (int)serialBufBegin[1], (int)serialBufBegin[2],
	//(int)serialBufBegin[3], (int)serialBufBegin[4], (int)serialBufBegin[5], (int)serialBufBegin[6]);
	res=(Send(&*serialBufBegin, serialBufSize));
	if (res != 0) {
	    ec->printf("[Server.Run]--> Send returned error code %d\n", res);
	    *ec << "[SERVER]--> ends with ERROR";
	    return ErrServer+ESend;
	}
		
}
	*ec << "[Server.Run]--> Releasing message buffers";
	free(messgBuff);
	
	//TODO DESTROY ALL OBJECTS
	*ec << "[Server.Run]--> Destroying Objects";
	//qEx->~QueryExecutor(); //segfault
	//*ec << "Past Executor";
	/*qPa->~QueryParser();
	*ec << "Past Parser";
	TransactionManager::getHandle()->~TransactionManager();
	*ec << "Past Transaction Manager";
	LockManager::getHandle()->~LockManager();
	*ec << "Past LockManager";
	//storeManager->~DBStoreManager(); //segfault
	// *ec << "Past DBStoreManager";
	logManager->~LogManager();
	*ec << "Past LogManager";
	*/
	*ec << "[Server.Run]--> Disconnecting";
	Disconnect();
	
	*ec << "[Server.Run]--> <><><>End<><><>"; 
	
	return 0;	
}		

int Server::SExit(int code) {
    ec->printf("[Server.SExit]--> Server thread freeing buffers and disconnecting - returning code |%d|\n", code);
    free(serialBufBegin);
    free(messgBuff);
    Disconnect();
    ec->printf("[Server.SExit]--> Server THREAD nr |%d| CLEANUP COMPLETE, returning\n", pthread_self());
    return code;
}

