
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <signal.h>

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



volatile sig_atomic_t signalReceived = 0;

void sigHandler(int sig);

Server::Server(int newSock)
{
	Sock = newSock;
}

Server::~Server()
{
}

//TODO - rekurencja, support innych typow
int  Server::Serialize(QueryResult *qr, char **buffer, char **bufStart) 
{
	QueryResult *collItem;
	QueryStringResult *stringRes;
	QueryBagResult *bagRes;
	QueryReferenceResult *refRes;
	QueryIntResult *intRes;
	QueryBoolResult *boolRes;
	QueryDoubleResult *doubleRes;
	
	Result::ResultType resType;

	int valSize;
	int bagSize;
	unsigned long bufBagLen;

	unsigned int intVal;
	double doubleVal;
	string strVal;

	char bufferP[MAX_MESSG];
	char *bufPointer;
	
	int retVal;
	int i;

	bufPointer = (char *)malloc(MAX_MESSG);
	memset(bufferP, '\0', MAX_MESSG); 
	bufPointer=bufferP;

	resType=(Result::ResultType)qr->type();
	
	printf("[Server.Serialize]--> Starting \n");
	printf("[Server.Serialize]--> Serializing object of type: %d \n", resType);
	
	switch (resType) {
		case Result::BAG:
			printf("[Server.Serialize]--> Received BAG ");
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
			
			printf("[Server.Serialize]--> Adding bag header \n");
			bufPointer[0]=(char)resType; //resType < 255
			bufPointer++;
			memcpy((void *)bufPointer, (const void *)&bufBagLen, sizeof(bufBagLen));
			printf("[Server.Serialize]--> Bag header complete \n");
			printf("[Server.Serialize]--> Bag header meaning is: |(type)=%d|(size)=%lu|\n", (int)bufferP[0], ntohl(*(unsigned long *)bufPointer));
			bufPointer=bufPointer+sizeof(bufBagLen);
			
									
			//TODO depth handling - rekurencja pewnie bedzie
			for (i=0;i<bagSize;i++) {
				printf("[Server.Serialize]--> Serializing collection item %d \n", i);
				retVal=bagRes->getResult(collItem);
				printf("[Server.Serialize]--> getResult returned %d \n", retVal);
			
				resType=(Result::ResultType)collItem->type();
				printf("[Server.Serialize]-->Result type %d \n", resType);
				if (resType==Result::REFERENCE) {
					bufPointer[0]=(char)resType;
					refRes = (QueryReferenceResult *)collItem;
					//intVal=(refRes->getValue())->toInteger();
					//strVal=((DBLogicalID *)(refRes->getValue()))->toString();
					strVal=(refRes->getValue())->toString();
					bufPointer++;
					printf("[Server.Serialize]-->Referencestring value: strVal=%s \n", strVal.c_str());
					printf("[Server.Serialize]-->This string is %d chars long\n", strVal.length());
					strVal.copy(bufPointer, strVal.length());
					//strcpy(bufPointer, "zak"); 
					//memcpy((void *)bufPointer, (const void *)&intVal, sizeof(intVal));
					bufPointer=bufPointer+strVal.length();
					bufPointer[0]='\0';
					bufPointer++;
				}
				if (resType==Result::INT) {
				    printf("[Server.Serialize]--> INT type, sending type indicator.. \n");
				    bufPointer[0]=(char)resType;
				    bufPointer++;
				    intRes = (QueryIntResult *)qr;
				    intVal = intRes->getValue();
				    printf("[Server.Serialize]--> .. followed by int value (%d) \n", intVal);
				    intVal=htonl(intVal);
				    memcpy((void *)bufPointer, (const void *)&intVal, sizeof(intVal));
				    printf("[Server.Serialize]--> buffer written \n");
				}
			}
			
				
				//switch (resTypeIns)
				//{
					//	case Result::STRING:
							//printf("[Server.Serialize]--> Getting string \n");
							//stringRes=(QueryStringResult *)collItem->clone();
							//strVal=stringRes->getValue();
							//valSize=stringRes->size();
							//printf("[Server.Serialize]--> Adding string header \n");
							//bufferP[0]=(char)resType;
							//*bufferP=*bufferP+1;
							//printf("[Server.Serialize]--> String header complete \n");
							//strVal.copy(bufferP, valSize);
			//				bufferP=bufferP + valSize;
						//	break;
					//	default:
							//printf("[Server.Serialize]--> Getting something else(!) \n");
					//		return -1;
					//		break;
					
				//}
			//}
			break;
		case Result::REFERENCE:
			printf("[Server.Serialize]--> Getting reference \n");
			bufPointer[0]=(char)resType;
			refRes = (QueryReferenceResult *)qr;
			intVal=(refRes->getValue())->toInteger();
			bufPointer++;
			printf("Reference integer value: intVal=%d \n", intVal);
			memcpy((void *)bufPointer, (const void *)&intVal, sizeof(intVal));
			bufPointer=bufPointer+sizeof(intVal);
			break;	
		case Result::STRING:
			printf("[Server.Serialize]--> Getting STRING \n");
			stringRes=(QueryStringResult *)qr;
			strVal=stringRes->getValue();
			valSize=stringRes->size();
			printf("[Server.Serialize]--> Adding string header \n");
			bufPointer[0]=(char)resType;
			bufPointer++;
			printf("[Server.Serialize]--> String header complete \n");
			strcpy(bufPointer, strVal.c_str());  //Czarek
			//strVal.copy(bufPointer, valSize);  //Czarek
			printf("[Server.Serialize]--> String serialized to: %s \n", bufPointer); //Czarek
			bufPointer=bufPointer+valSize;
			break;
		case Result::RESULT:
		    printf("[Server.Serialize]--> Getting RESULT (Getting WHAT?) \n");
		    break;    
		 case Result::VOID:
			printf("[Server.Serialize]--> VOID type, sending type indicator and C(orrect) (what should I?)\n");
			bufPointer[0]=(char)resType;
			//sprintf(bufferP, "%dC", (char)resType);
			printf("%d\n", (int)bufPointer[0]);
			printf("[Server.Serialize]--> VOID type written \n");
			printf("%d\n", (int)bufferP[0]);
			break;
		case Result::INT:
			printf("[Server.Serialize]--> INT type, sending type indicator.. \n");
		    bufPointer[0]=(char)resType;
		    bufPointer++;
		    intRes = (QueryIntResult *)qr;
		    intVal = intRes->getValue();
		    printf("[Server.Serialize]--> .. followed by int value (%d) \n", intVal);
			intVal=htonl(intVal);
		    memcpy((void *)bufPointer, (const void *)&intVal, sizeof(intVal));
		    printf("[Server.Serialize]--> buffer written \n");
		    break; 
		case Result::DOUBLE:
			//printf("[Server.Serialize]--> DOUBLE type, NOT IMLEMENTEND YET!!! \n");
		    bufPointer[0]=(char)resType;
		    bufPointer++;
		    doubleRes = (QueryDoubleResult *)qr;
		    doubleVal = doubleRes->getValue();
		    doubleVal=htonl(doubleVal);
		    memcpy((void *)bufPointer, (const void *)&doubleVal, sizeof(doubleVal));
		    
		    break; 
		case Result::BOOLTRUE:
			printf("[Server.Serialize]--> BOOLTRUE type, sending type indicator only\n");
			bufPointer[0]=(char)resType;
			break;
		case Result::BOOLFALSE:
			printf("[Server.Serialize]--> BOOLFALSE type, sending type indicator only\n");
			bufPointer[0]=(char)resType;
			break;
		case Result::BOOL:
			printf("[Server.Serialize]--> BOOL type, sending type indicator and value\n");
			bufPointer[0]=(char)resType;
			boolRes = (QueryBoolResult *)qr;
			bufPointer[1]=(char)(boolRes->getValue());
			break;
		default:
			printf("[Server.Serialize]--> object type not handled yet(!) \n");
			return -1;
			break;
	}

	char *finalBuf=(char *)malloc(MAX_MESSG);
	memcpy(finalBuf, bufferP, MAX_MESSG);
	printf("[Server.Serialize]--> Ending.. \n");
	printf("[Server.Serialize]--> Passing type=%d\n", (int)finalBuf[0]);
	*buffer = finalBuf;
	return 0;
}


int Server::Run()
{
	int size=MAX_MESSG;
	int res=0;
	
	sigset_t block_cc;
	
	printf("[Server.Run]--> Starts \n");

	//Signal Handling
	sigemptyset(&block_cc);
	sigaddset(&block_cc, SIGINT);
	
	//Create process signal mask
	sigprocmask(SIG_BLOCK, &block_cc, NULL);
	
	signal(SIGINT, sigHandler);

	ErrorConsole con;
	con.init(1);

	SBQLConfig* config = new SBQLConfig("Server");
	config->init();
	
	LogManager* logManager = new LogManager();
	logManager->init();
	
	DBStoreManager* storeManager = new DBStoreManager();
	storeManager->init(config, logManager);
	
	TransactionManager::getHandle()->init(storeManager, logManager);
	
	QueryParser *qPa = new QueryParser();
	QueryExecutor *qEx = new QueryExecutor();
		
	TreeNode *tNode;
	QueryResult *qResult;
	
	printf("[Server.Run]--> Creating message buffers \n");
	char *messgBuff;
	char *serializedMessg;
	char **sPoint;
	messgBuff=(char*) malloc(MAX_MESSG);
	
	serializedMessg=(char*) malloc(MAX_MESSG);
	
	sigprocmask(SIG_UNBLOCK, &block_cc, NULL);
	
	
while (!signalReceived) {

	printf("[Server.Run]--> Blocking signals \n");
	sigprocmask(SIG_BLOCK, &block_cc, NULL);
	
	printf("[Server.Run]--> Cleaning buffers \n");
	memset(serializedMessg, '\0', MAX_MESSG); 
//	*sPoint=serializedMessg;	//piotrek - nie uzywane a pod cygwinem naruszenie ochrony pamieci
	
	//Get string from client
	printf("[Server.Run]--> Receiving query from client \n");
	sigprocmask(SIG_UNBLOCK, &block_cc, NULL);
	
	res = (Receive(&messgBuff, &size));
	if (res < 0) {
	    printf("[Server.Run--> Receive returned error code %d\n", res);
	    return ErrServer+EReceive;
	}    
	
	sigprocmask(SIG_BLOCK, &block_cc, NULL);	
	
	printf("[Server.Run]--> Request parse \n");
	printf("string do parsera: %s\n", messgBuff);
	cout << (string) messgBuff << endl;
	res = (qPa->parseIt((string) messgBuff, tNode));
	if (res < 0) {
	    printf("[Server.Run--> Parser returned error code %d\n", res);
	    return ErrServer+EParse;
	}
	
	printf("tree node%d\n", (int) tNode); 
	printf("[Server.Run]--> Request query result \n");
	res = (qEx->executeQuery(tNode, &qResult)); 
	if (res < 0) {
	    printf("[Server.Run--> Executor returned error code %d\n", res);
	    return ErrServer+EExecute;
	} 
	
	printf("[Server.Run]--> Serializing data \n");
	printf ("[Server.Run]-->qResult=%d \n", (int)qResult);
	
	if (qResult == 0) {printf ("brak wyniku\n"); return 0;} //Piotrek
	
	res = (Serialize(qResult, (char **)&serializedMessg, sPoint)); 
	if (res < 0) {
	    printf("[Server.Run--> Serialize returned error code %d\n", res);
	    return ErrServer+ESerialize;
	}
	
	//Send results to client
	printf("[Server.Run]--> Sending results to client \n");		
	printf("[Server.Run]--> Sending.. type=(%d)\n", (int)serializedMessg[0]); 
	res=(Send(&*serializedMessg, MAX_MESSG));
	if (res < 0) {
	    printf("[Server.Run--> Send returned error code %d\n", res);
	    return ErrServer+ESend;
	}
	
	sigprocmask(SIG_UNBLOCK, &block_cc, NULL);
}
	printf("[Server.Run]--> Releasing message buffers \n");
	free(messgBuff);
	free(serializedMessg);
	
	printf("[Server.Run]--> destroying TransactionManager \n");
	TransactionManager::getHandle()->~TransactionManager();
	LockManager::getHandle()->~LockManager();
	printf("[Server.Run]--> Disconnecting \n");
	Disconnect();
	
	printf("[Server.Run]--> Ends \n"); 
	
	return 0;	
}		

int BExit() {
	printf("[Server.BExit]-->destroying TM\n");	
	TransactionManager::getHandle()->~TransactionManager();
	printf("[Server.BExit]-->destroying LM\n");
	LockManager::getHandle()->~LockManager();
	//printf("[Server.BExit]-->disconnecting\n");
	//Disconnect();
	printf("[Server.BExit]-->exiting\n");
	exit(0);
	return 0;
}

void sigHandler(int sig) {
	printf("[Server]--> Signal intercepted \n");
	signalReceived = 1;    
	signal(sig, sigHandler);
	printf("[Server]--> Quitting signal handler, calling BExit function \n");
}

