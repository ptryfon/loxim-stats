
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include "../Config/SBQLConfig.h"
#include "../Log/Logs.h"
#include "../QueryParser/QueryParser.h"
#include "../QueryParser/TreeNode.h"
#include "../QueryExecutor/QueryResult.h"
#include "../QueryExecutor/QueryExecutor.h"
#include "../Store/DBStoreManager.h"
#include "../TransactionManager/Transaction.h"
#include "../Errors/ErrorConsole.h"

#include "Server.h"
#include "../Driver/Result.h"

typedef char TREE_NODE_TYPE;


// TODO 
// 1-Inicjalizacja do Listenera
// 2-Watki
// 3-Obsluga bledow
// 4-Lepsza diagnostyka

/*
#include "../QueryParser/QueryParser.h"

//Co za shit ;)
#include "../QueryExecutor/QueryResult.h"

#include "../QueryExecutor/QueryExecutor.h"

#include "../Config/SBQLConfig.h"
#include "../Log/Logs.h"

#include "../Store/DBStoreManager.h"
#include "../TransactionManager/Transaction.h"
*/

using namespace Errors;
using namespace std;
using namespace Config;
using namespace Logs;
using namespace Store;
using namespace QParser;
using namespace QExecutor;
using namespace TManager;


Server::Server(int newSock)
{
	Sock = newSock;
	//memset(messgBuff, 0, sizeof(messgBuff));
}

Server::~Server()
{
}


//TODO - inicjacja obiektow
int Server::InitializeAll() 
{
	return 0;
}

//TODO - rekurencja, support innych typow
int  Server::Serialize(QueryResult *qr, char **buffer, char **bufStart) 
{
	//int depth=SERIALIZE_DEPTH;
	QueryResult *collItem;
	QueryStringResult *stringRes;
	QueryBagResult *bagRes;
	//cenzura
	Result::ResultType resType;
	resType=(Result::ResultType)qr->type();
	Result::ResultType resTypeIns;
	int valSize;
	int bagSize;
	string strVal;
	unsigned int intVal;
	unsigned long bufBagLen;
	char bufferP[MAX_MESSG];
	char *bufPointer;
	char sentType[1];
	sentType[0]='\0';
	char *finalBuf;
	int retVal;
	char * dupaTest="dupa";
	QueryReferenceResult *refRes;
	
	
	finalBuf = (char *)malloc(MAX_MESSG);
	memset(bufferP, '\0', MAX_MESSG); 
	bufPointer=bufferP;
		
	//printf("[Server.Serialize]--> Starting 1\n")
	
	//int i;
	printf("[Server.Serialize]--> Starting \n");
	printf("[Server.Serialize]--> Serializing object of type: %d \n", resType);
	//printf("[Server.Serialize]-->Sizeof char %d, sizeof ResultType %d \n", sizeof(char), sizeof(Result::ResultType));
	
	switch (resType) {
		case Result::BAG:
			printf("[Server.Serialize]--> Received BAG ");
			bagSize=qr->size();
			bufBagLen=(unsigned long) bagSize;
			printf(" of size %d=%lu \n", bagSize, bufBagLen);
			bufBagLen=htonl(bufBagLen);
			if (qr->collection() != true) {
				printf("[Server.Serialize]-->QueryResult shows type of BAG and says it is no collection\n");
				return -1;
			} //TODO errorcode
			bagRes = (QueryBagResult *)qr->clone();
			
			printf("[Server.Serialize]--> Adding bag header \n");
			bufPointer[0]=(char)resType;
			bufPointer++;
			memcpy((void *)bufPointer, (const void *)&bufBagLen, sizeof(bufBagLen));
			printf("[Server.Serialize]--> Bag header complete \n");
			printf("[Server.Serialize]--> Bag size as passed is |(size)=%lu|\n", ntohl(*(unsigned long *)bufPointer));
			bufPointer=bufPointer+sizeof(bufBagLen);
												
			//TODO depth handling - rekurencja pewnie bedzie
			printf("[Server.Serialize]--> Getting collection items \n");
			//while (bagRes->getResult(collItem)!=-1) {
				retVal=bagRes->getResult(collItem);
				printf("[Server.Serialize]--> getResult returned %d \n", retVal);
				printf("[Server.Serialize]--> Item type is |%d| \n", collItem->type());
				//contains no more results
				resTypeIns=(Result::ResultType)collItem->type();
				printf("[Server.Serialize]-->Result type %d \n", resTypeIns);
				if (resTypeIns==Result::REFERENCE) {
					bufPointer[0]=(char)resTypeIns;
					printf("Checker -1\n");
					refRes = (QueryReferenceResult *)collItem;
					intVal=(refRes->getValue())->toInteger();
					bufPointer++;
					printf("Checker \n");
					memcpy((void *)bufPointer, (const void *)&intVal, sizeof(intVal));
					bufPointer=bufPointer+sizeof(intVal);
					printf("Checker 1\n");
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
			break;	
		case Result::STRING:
			printf("[Server.Serialize]--> Getting string \n");
			stringRes=(QueryStringResult *)collItem->clone();
			strVal=stringRes->getValue();
			valSize=stringRes->size();
			printf("[Server.Serialize]--> Adding string header \n");
			bufPointer[0]=(char)resType;
			bufPointer++;
			printf("[Server.Serialize]--> String header complete \n");
			strVal.copy(bufferP, valSize);
			bufPointer=bufPointer + valSize;
			break;
		case Result::RESULT:
			printf("\n[Server.Serialize]--> BAD TYPE RECEIVED FROM EXECUTOR -- RESULT \n \n");
		    printf("[Server.Serialize]--> Getting RESULT (Getting WHAT?) \n");
		    printf("[Server.Serialize]--> Sending sth for testing instead.. \n");
		    //printf("[Server.Serialize]--> Adding some header \n");
		   // memcpy(buffer, (char *)resType, 4);
		   // printf("[Server.Serialize]--> in progress.. \n");
		   // printf("[Server.Serialize]--> Some header added \n"); 
		    memcpy(bufPointer, dupaTest, strlen(dupaTest));
		   // memcpy(&buffer, '\0', 1);
		    break;    
		 case Result::VOID:
			printf("[Server.Serialize]--> VOID type, sending type indicator and C(orrect) (what should I?)\n");
			bufPointer[0]=(char)resType;
			//sprintf(bufferP, "%dC", (char)resType);
			printf("%d\n", (int)bufPointer[0]);
			printf("[Server.Serialize]--> VOID type written \n");
			printf("%d\n", (int)bufferP[0]);
			break;
		default:
			printf("[Server.Serialize]--> object type not handled yet(!) \n");
			return -1;
			break;
	}
	//free(bufPointer);
	printf("[Server.Serialize]--> Done! \n");
	memcpy(finalBuf, bufferP, MAX_MESSG);
	printf("[Server.Serialize]--> Ending.. \n");
	printf("bufferType=%d\n", (int)(bufferP[0]));
	*buffer = finalBuf;
	//printf("%d\n", (int)*buffer[0]);
	//printf("[Server.Serialize]--> I've got a nice buffer containing:  \n--->%s<---\n", *buffer);
	//printf("...ok, it's not so nice, but first char is --->%d<---\n", (int)*buffer[0]); 
	return 0;
}


int Server::Run()
{
	int size=MAX_MESSG;
	//TODO - headers
	//TREE_NODE_TYPE type;
	//type=1;
	
	
	printf("[Server.Run]--> Starts \n");

	ErrorConsole con;// Czarek
	con.init(1);     // Czarek

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
	
while (true) {

	printf("[Server.Run]--> Cleaning buffers \n");
	memset(serializedMessg, '\0', MAX_MESSG); 
	*sPoint=serializedMessg;	
	
	//Get string from client
	printf("[Server.Run]--> Receiving query from client \n");
	Receive(&messgBuff, &size);
		
	printf("[Server.Run]--> Request parse \n");
	printf("string do parsera: %s\n", messgBuff);
	cout << (string) messgBuff << endl;
	qPa->parseIt((string) messgBuff, tNode); 
	
	printf("tree node%d\n", (int) tNode); 
	printf("[Server.Run]--> Request query result \n");
	qEx->executeQuery(tNode, &qResult); 
	 
	printf("[Server.Run]--> Serializing data \n");
	printf ("[Server.Run]-->qResult=%d \n", (int)qResult);
	
	if (qResult == 0) {printf ("brak wyniku\n"); return 0;} //Piotrek
	
	Serialize(qResult, (char **)&serializedMessg, sPoint); 
	
	
	//Send results to client
	printf("[Server.Run]--> Sending results to client \n");		
	printf("[Server.Run]--> Sending.. type=(%d)\n", (int)serializedMessg[0]); 
	Send(serializedMessg, MAX_MESSG);
}
	printf("[Server.Run]--> Releasing message buffers \n");
	//TODO
	
	printf("[Server.Run]--> destroying TransactionManager \n");
	TransactionManager::getHandle()->~TransactionManager();
	LockManager::getHandle()->~LockManager();
	printf("[Server.Run]--> Disconnecting \n");
	Disconnect();
	
	printf("[Server.Run]--> Ends \n");
	return 0;	
}		


