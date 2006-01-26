
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

#include "../Config/SBQLConfig.h"
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
using namespace Config;
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
}

Server::~Server()
{
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
	
	//printf("[Server.Serialize]--> Starting \n");
	printf("[Server.Serialize]--> Now serializing object of type: %d \n",  rT);
	
	switch (rT) {
		case QueryResult::QBAG:
			printf("[Server.Serialize]--> Received BAG ");
			resType=Result::BAG;
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			printf(" of size %lu \n", bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				printf("[Server.Serialize]-->QueryResult shows type of BAG and says it is no collection\n");
				return ErrServer+EBadResult;
				return -1;
			} 
			bagRes = (QueryBagResult *)qr;	
			//printf("[Server.Serialize]--> Adding bag header \n");
			serialBuf[0]=(char)resType;
			//printf("\n SerialBuf[0] |%c|%d| \n\n", serialBuf[0], (int)serialBuf[0]);
			//printf("\n SerialBufBegin[0] |%c|%d| \n\n", serialBufBegin[0], (int)serialBufBegin[0]);
			serialBuf++;
			//printf("\n SerialBuf[0] |%s|%d| \n\n", serialBuf[0], (int)serialBuf[0]);
			//printf("\n SerialBufBegin[0] |%c|%d| \n\n", serialBufBegin[0], (int)serialBufBegin[0]);
			
			memcpy((void *)serialBuf, (const void *)&bufBagLen, sizeof(bufBagLen));
			printf("[Server.Serialize]--> Bag header: |(type)=%d|(size)=%lu|\n", (int)serialBufBegin[0], ntohl(*(unsigned long *)serialBuf));
			serialBuf=serialBuf+sizeof(bufBagLen);
						
			//TODO DEPTH
			for (i=0;i<bagSize;i++) {
				printf("[Server.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				if (retVal!=0) {
				    printf("[Server.Serialize]--> getResult failed with %d, returning.. \n", retVal);
				    return retVal;
				}
				SerializeRec(collItem);
			}
			break;
		case QueryResult::QSEQUENCE: //TODO CZYM SIE ROZNI SEQUENCE OD BAGA??
			printf("[Server.Serialize]--> Received SEQUENCE ");
			resType=Result::SEQUENCE;
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			printf(" of size %lu \n", bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				printf("[Server.Serialize]-->QueryResult shows type of SEQUENCE and says it is no collection\n");
				return ErrServer+EBadResult;
				return -1;
			} 
			bagRes = (QueryBagResult *)qr;
			printf("[Server.Serialize]--> Adding sequence header \n");
			serialBuf[0]=(char)resType;
			serialBuf++;
			memcpy((void *)serialBuf, (const void *)&bufBagLen, sizeof(bufBagLen));
			//printf("[Server.Serialize]--> Sequence header: |(type)=%d|(size)=%lu|\n", (int)serialBufBegin[0], ntohl(*(unsigned long *)serialBuf));
			serialBuf=serialBuf+sizeof(bufBagLen);
						
			//TODO DEPTH
			for (i=0;i<bagSize;i++) {
				printf("[Server.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				if (retVal!=0) {
				    printf("[Server.Serialize]--> getResult failed with %d, returning.. \n", retVal);
				    return retVal;
				}
				SerializeRec(collItem);
			}
				
			break;
		case QueryResult::QSTRUCT: //TODO CZYM SIE ROZNI STRUCT OD BAGA??
			printf("[Server.Serialize]--> Received STRUCT ");
			resType=Result::STRUCT;
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			printf(" of size %lu \n", bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				printf("[Server.Serialize]-->QueryResult shows type of STRUCT and says it is no collection\n");
				return ErrServer+EBadResult;
			} 
			bagRes = (QueryBagResult *)qr;
			printf("[Server.Serialize]--> Adding struct header \n");
			serialBuf[0]=(char)resType;
			serialBuf++;
			memcpy((void *)serialBuf, (const void *)&bufBagLen, sizeof(bufBagLen));
			//printf("[Server.Serialize]--> Struct header: |(type)=%d|(size)=%lu|\n", (int)serialBufBegin[0], ntohl(*(unsigned long *)serialBuf));
			serialBuf=serialBuf+sizeof(bufBagLen);
						
			//TODO DEPTH
			for (i=0;i<bagSize;i++) {
				printf("[Server.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				if (retVal!=0) {
				    printf("[Server.Serialize]--> getResult failed with %d, returning.. \n", retVal);
				    return retVal;
				}
				SerializeRec(collItem);
			}
				
			break;
		case QueryResult::QREFERENCE:
		    printf("[Server.Serialize]--> Getting reference \n");
		    resType=Result::REFERENCE;
		    serialBuf[0]=(char)resType;
		    refRes = (QueryReferenceResult *)qr;
		    intVal=(refRes->getValue())->toInteger();
		    serialBuf++;
		    printf("[Server.Serialize]--> Reference integer value: intVal=%d \n", intVal);
		    memcpy((void *)serialBuf, (const void *)&intVal, sizeof(intVal));
		    serialBuf=serialBuf+sizeof(intVal);
		    break;	
		case QueryResult::QSTRING:
		    printf("[Server.Serialize]--> Getting STRING \n");
		    resType=Result::STRING;
		    stringRes=(QueryStringResult *)qr;
		    strVal=stringRes->getValue();
		    valSize=strVal.length();
		    printf("[Server.Serialize]--> Adding string header, string size=%d \n", valSize);
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    //printf("[Server.Serialize]--> String header complete \n");
		    strcpy(serialBuf, strVal.c_str());  
		    printf("[Server.Serialize]--> String serialized to: %s \n", serialBuf);
		    serialBuf=serialBuf+valSize+1;
		    break;
		case QueryResult::QRESULT:
		    printf("[Server.Serialize]--> Getting RESULT (ERROR!)\n");
		    return -1; //TODO ERRORCODE
		    break;    
		 case QueryResult::QNOTHING:
		    printf("[Server.Serialize]--> VOID type, sending type indicator only\n");
		    resType=Result::VOID;
		    serialBuf[0]=(char)resType;
		    printf("[Server.Serialize]--> VOID type written \n");
		    serialBuf++;
		    break;
		case QueryResult::QINT:
		    printf("[Server.Serialize]--> INT type, sending type indicator.. \n");
		    resType=Result::INT;
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    intRes = (QueryIntResult *)qr;
		    intVal = intRes->getValue();
		    printf("[Server.Serialize]--> .. followed by int value (%d) \n", intVal);
		    intVal=htonl(intVal);
		    memcpy((void *)serialBuf, (const void *)&intVal, sizeof(intVal));
		    serialBuf=serialBuf+sizeof(intVal);
		    //printf("[Server.Serialize]--> buffer written \n");
		    break; 
		case QueryResult::QDOUBLE:
		    printf("[Server.Serialize]--> DOUBLE type \n");
		    resType=Result::DOUBLE;
		    serialBuf[0]=(char)resType;
		    serialBuf++;
		    doubleRes = (QueryDoubleResult *)qr;
		    doubleVal = doubleRes->getValue();
		    printf("[Server.Serialize]--> double value is %lf \n", doubleVal);
		    //printf("[Server.Serialize]--> Switching double byte order.. \n");
		    doubleVal = htonDouble(doubleVal);
		    printf("[Server.Serialize]--> Splitting and byte order change complete \n");
		    memcpy((void *)serialBuf, (const void *)&doubleVal, sizeof(doubleVal));
		    serialBuf = serialBuf + sizeof(doubleVal);		        
		    break; 
		case QueryResult::QBOOL: //TODO - pass either BOOLTRUE or BOOLFALSE or just BOOL?
		    printf("[Server.Serialize]--> BOOL type, sending type indicator and value\n");
		    boolRes=(QueryBoolResult *)qr;
		    boolVal=boolRes->getValue();
		    if (boolVal==false)
		        resType=Result::BOOLFALSE;
		    else if (boolVal==true)
			resType=Result::BOOLTRUE;
		    else {
			printf("[Server.Serialize]-->Bool result from Executor corrupted: |%d| \n", (int)boolVal);
			printf("[Server.Serialize]-->Sending FALSE.. \n");
			resType=Result::BOOLFALSE;
		    }
		    serialBuf[0]=(char)resType;
		    boolRes = (QueryBoolResult *)qr;
		    serialBuf++;
		    serialBuf[0]=(char)(boolVal);
		    serialBuf++;		    
		    printf("[Server.Serialize]--> Sending bool: |type=%d|value=%d|\n", (int)resType, (int)boolVal);
		    break;
		case QueryResult::QBINDER:
		    printf("[Server.Serialize]--> BINDER type, sending name(string) and result\n");
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
		    printf("[Server.Serialize]--> object UNRECOGNIZED ! \n");
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
	
	printf("[Server.Run]--> Starts \n");
	
	//Signal Handling
	sigemptyset(&block_cc);
	sigaddset(&block_cc, SIGINT);
//	signal(SIGINT, (Server::sigHandler));
	
	//Set process signal mask
	//printf("[Server.Run]--> Blocking SIGINT for now.. \n");
	sigprocmask(SIG_BLOCK, &block_cc, NULL);

	//TODO same objects as Listener 
	printf("[Server.Run]--> Initializing objects.. \n");
	SBQLConfig* config = new SBQLConfig("Server");
	config->init();
	ErrorConsole con("Server");
	con.init(1);

		
	LogManager* logManager = new LogManager();
	logManager->init();
	DBStoreManager* storeManager = new DBStoreManager();
	storeManager->init(config, logManager);
	TransactionManager::getHandle()->init(storeManager, logManager);
	storeManager->start();
	//storeManager->setTransactionManager(TransactionManager::getHandle());
	
	QueryParser *qPa = new QueryParser();
	QueryExecutor *qEx = new QueryExecutor();
		
	TreeNode *tNode;
	QueryResult *qResult;
	
	printf("[Server.Run]--> Creating message buffers.. \n");
	
	messgBuff=(char*) malloc(MAX_MESSG);

	serialBufBegin=(char *)malloc(MAX_MESSG);
	serialBufEnd=serialBufBegin+MAX_MESSG;
	serialBufSize=MAX_MESSG;
	serialBuf=serialBufBegin;
	
	
while (!signalReceived) {

	if (ncError==1)
	    printf("[Server.Run]-> a non-critical error occured during the previous session \n");
    	memset(serialBufBegin, '\0', MAX_MESSG);
	serialBuf=serialBufBegin;	
	ncError=0;

	printf("[Server.Run]--> Blocking SIGINT for now.. \n");
	sigprocmask(SIG_BLOCK, &block_cc, NULL);
	
	//printf("[Server.Run]--> Cleaning buffers \n");
	//memset(serializedMessg, '\0', MAX_MESSG); 
	
	//Get string from client
	printf("[Server.Run]--> Unblocking sigint and receiving query from client \n");
	sigprocmask(SIG_UNBLOCK, &block_cc, NULL);
	
	res = (Receive(&messgBuff, &size));
	if (res != 0) {
	    printf("[Server.Run--> Receive returned error code %d\n", res);
	    //sendError(res);
	    return res;
	}    
	if (messgBuff==NULL) {
	    printf("[Server.Run]--> Error in receive, client terminated??\n");
	    printf("[Server.Run]--> Assuming client loss for that thread - TERMINATING thread\n");
	    return ErrServer + EClientLost; //TODO Error
	}

	printf("[Server.Run]--> Blocking sigint again.. \n");
	sigprocmask(SIG_BLOCK, &block_cc, NULL);	
	
	printf("[Server.Run]--> Requesting PARSE: |%s| \n", messgBuff);
	res = (qPa->parseIt((string) messgBuff, tNode));
	if (res != 0) {
	    printf("[Server.Run]--> Parser returned error code %d\n", res);
	    sendError(res);
	    ncError=1;
	    continue;
	    printf("[SERVER]--> ends with ERROR\n");
	    return res;
	}
	
	printf("[Server.Run]--> Requesting EXECUTE on tree node: |%d| \n", (int) tNode);
	res = (qEx->executeQuery(tNode, &qResult)); 
	if (res != 0) {
	    printf("[Server.Run]--> Executor returned error code %d\n", res);
	    sendError(res);
	    ncError=1;
	    continue;
	    printf("[SERVER]--> ends with ERROR\n");
	    return ErrServer+EExecute;
	} 
	
	printf("[Server.Run]--> EXECUTE complete, checking results.. \n");
	printf("[Server.Run]--> Got qResult=|%d| of size: qResult->size()=|%d| :\n",  (int)qResult, qResult->size());
	
	if (qResult == 0) {//TODO ERROR
	 return 0;} 
	
	printf("[Server.Run]--> *****SERIALIZE starts***** \n");
	
	res = (SerializeRec(qResult)); 
	if (res != 0) {
	    printf("[Server.Run--> Serialize returned error code %d\n", res);
	    sendError(res);
	    printf("[SERVER]--> ends with ERROR\n");
	    return ErrServer+ESerialize;
	}
	
	printf("[Server.Run]--> *****SERIALIZE complete***** \n");
	
	printf("[Server.Run]--> Sending results to client.. Result type=|%d|\n", (int)serialBufBegin[0]);
	//printf("Buf: |%d|%d|%d|%d|%d|%d|%d|\n", (int)serialBufBegin[0], (int)serialBufBegin[1], (int)serialBufBegin[2],
	//(int)serialBufBegin[3], (int)serialBufBegin[4], (int)serialBufBegin[5], (int)serialBufBegin[6]);
	res=(Send(&*serialBufBegin, serialBufSize));
	if (res != 0) {
	    printf("[Server.Run]--> Send returned error code %d\n", res);
	    printf("[SERVER]--> ends with ERROR\n");
	    return ErrServer+ESend;
	}
		
}
	printf("[Server.Run]--> Releasing message buffers \n");
	free(messgBuff);
	
	//TODO DESTROY ALL OBJECTS
	printf("[Server.Run]--> Destroying Objects \n");
	//qEx->~QueryExecutor(); //segfault
	//printf("Past Executor\n");
	/*qPa->~QueryParser();
	printf("Past Parser\n");
	TransactionManager::getHandle()->~TransactionManager();
	printf("Past Transaction Manager\n");
	LockManager::getHandle()->~LockManager();
	printf("Past LockManager\n");
	//storeManager->~DBStoreManager(); //segfault
	//printf("Past DBStoreManager\n");
	logManager->~LogManager();
	printf("Past LogManager\n");
	con.~ErrorConsole();
	printf("Past ErrorConsole\n");
	config->~SBQLConfig();
	printf("Past SBQLConfig\n");
	*/
	printf("[Server.Run]--> Disconnecting \n");
	Disconnect();
	
	printf("[Server.Run]--> <><><>End<><><> \n"); 
	
	return 0;	
}		

int Server::SExit(int code) {
    printf("[Server.SExit]--> Server thread freeing buffers and disconnecting - returning code |%d|\n", code);
    free(serialBufBegin);
    free(messgBuff);
    Disconnect();
    printf("[Server.SExit]--> Server THREAD nr |%d| CLEANUP COMPLETE, returning\n", pthread_self());
    return code;
}

