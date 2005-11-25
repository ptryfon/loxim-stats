
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

#include "Server.h"
#include "../Driver/Result.h"

typedef char TREE_NODE_TYPE;

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
int  Server::Serialize(QueryResult *qr, char *buffer) 
{
	//int depth=SERIALIZE_DEPTH;
	QueryResult *collItem;
	QueryStringResult *stringRes;
	QueryBagResult *bagRes;
	Result::ResultType resType;
	resType=(Result::ResultType)qr->type();
	int valSize;
	int bagSize;
	string strVal;
	unsigned long bufBagLen;
	//int i;
	printf("[Server.Serialize]--> Starting \n");
	printf("[Server.Serialize]-->Sizeof char %d, sizeof ResultType %d \n", sizeof(char), sizeof(Result::ResultType));
	
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
			}
			bagRes = (QueryBagResult *)qr->clone();
			
			printf("[Server.Serialize]--> Adding bag header \n");
			memcpy(buffer, (void *)resType, 1);
			buffer=buffer+1;
			memcpy(buffer, (void *)bufBagLen, sizeof(unsigned long));
			printf("[Server.Serialize]--> Bag header complete \n");
												
			//TODO depth handling
			printf("[Server.Serialize]--> Getting collection items \n");
			while (bagRes->getResult(collItem)!=-1) {
				if (collItem->isEmpty()==true) {
					//contains no more results
					switch (collItem->type()) {
						case Result::STRING:
							printf("[Server.Serialize]--> Getting string \n");
							stringRes=(QueryStringResult *)collItem->clone();
							strVal=stringRes->getValue();
							valSize=stringRes->size();
							printf("[Server.Serialize]--> Adding string header \n");
							memcpy(buffer, (void *)resType, 1);
							buffer=buffer+1;
							printf("[Server.Serialize]--> String header complete \n");
							strVal.copy(buffer, valSize);
							buffer=buffer + valSize;
							break;
						default:
							printf("[Server.Serialize]--> Getting something else(!) \n");
							return -1;
							break;
					}
				}
				else {
					printf("[Server.Serialize]--> Deep collections not handled yet(!) \n");
					return -1;
				}
			}
			break;
		case Result::STRING:
			printf("[Server.Serialize]--> Getting string \n");
			stringRes=(QueryStringResult *)collItem->clone();
			strVal=stringRes->getValue();
			valSize=stringRes->size();
			printf("[Server.Serialize]--> Adding string header \n");
			memcpy(buffer, (void *)resType, 1);
			buffer=buffer+1;
			printf("[Server.Serialize]--> String header complete \n");
			strVal.copy(buffer, valSize);
			buffer=buffer + valSize;
			break;
		default:
			printf("[Server.Serialize]--> object type not handled yet(!) \n");
			return -1;
			break;
	}
	printf("[Server.Serialize]--> Done! \n");			
	return 0;
}


int Server::Run()
{
	int size=MAX_MESSG;
	//TODO - headers
	TREE_NODE_TYPE type;
	type=1;
	
	printf("[Server.Run]--> Starts \n");
	SBQLConfig* config = new SBQLConfig();
	config->init();
	
	LogManager* logManager = new LogManager();
	logManager->init();
	
	DBStoreManager* storeManager = new DBStoreManager();
	storeManager->init(config, logManager);
	
	TransactionManager::getHandle()->init(storeManager);
	
	QueryParser *qPa = new QueryParser();
	QueryExecutor *qEx = new QueryExecutor();
		
	TreeNode *tNode;
	QueryResult *qResult;
	
	printf("[Server.Run]--> Creating message buffers \n");
	char messgBuff[MAX_MESSG];
	char serializedMessg[MAX_MESSG+MAX_MESSG_HEADER_LEN];
	memset(messgBuff, '\0', MAX_MESSG);
	memset(serializedMessg, 0, MAX_MESSG+MAX_MESSG_HEADER_LEN);
	
	
	//Get string from client
	printf("[Server.Run]--> Receiving query from client \n");
	Receive((char **)messgBuff, &size);
		
	printf("[Server.Run]--> Request parse \n");
	qPa->parseIt((string) messgBuff, tNode); //by reference
	
	printf("tree node%d", (int) tNode);
	printf("[Server.Run]--> Request query result \n");
	qEx->executeQuery(tNode, &qResult);
	 
	printf("[Server.Run]--> Serializing data \n");
	printf ("%d", (int)qResult);
	
	if (qResult == 0) {printf ("brak wyniku\n"); return 0;}
	
	Serialize(qResult, (char *)serializedMessg);
	
	//Send results to client
	printf("[Server.Run]--> Sending results to client \n");		
	Send(serializedMessg, MAX_MESSG);
	
	printf("[Server.Run]--> Releasing message buffers \n");
	free(messgBuff);
	free(serializedMessg);
	
	printf("[Server.Run]--> Disconnecting \n");
	Disconnect();
	
	printf("[Server.Run]--> Ends \n");
	return 0;	
}		


