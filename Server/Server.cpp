#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <string>
#include "../Config/SBQLConfig.h"
#include "../Log/Logs.h"
#include "../QueryParser/QueryParser.h"
#include "../QueryParser/TreeNode.h"
#include "../QueryExecutor/QueryResult.h"
#include "../QueryExecutor/QueryExecutor.h"
#include "../Store/DBStoreManager.h"
#include "../TransactionManager/Transaction.h"

#include "Server.h"

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
int Server::initializeAll() 
{
	return 0;
}

// Stary kod z CT.cpp - nie wglebialem sie
int Server::Run()
{
	int size=128;
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
	QueryResult *qResult = new QueryResult();
	
	printf("[Server.Run]--> Creating message buffers \n");
	char messgBuff[MAX_MESSG];
	char serializedMessg[MAX_MESSG+MAX_MESSG_HEADER_LEN];
	memset(messgBuff, 0, MAX_MESSG);
	memset(serializedMessg, 0, MAX_MESSG+MAX_MESSG_HEADER_LEN);
	
	
	//Get string from client
	printf("[Server.Run]--> Receiving query from client \n");
	Receive((char **)messgBuff, &size);
		
	printf("[Server.Run]--> Request parse \n");
	tNode=qPa->parse((string) messgBuff);
	
	printf("[Server.Run]--> Request query result \n");
	qEx->queryResult(tNode, qResult);
	
	printf("[Server.Run]--> Serializing data \n");
	
	//Setting type
	serializedMessg[0]=type;
	
	//Case type switch should be here
	
	memcpy(serializedMessg+1, qResult, sizeof(qResult)); 
	
	
	//Send results to client
	printf("[Server.Run]--> Sending results to client \n");		
	Send(serializedMessg, MAX_MESSG);
	
	//That's it, we no longer need this one..
	printf("[Server.Run]--> Releasing message buffers \n");
	free(messgBuff);
	free(serializedMessg);
	
	printf("[Server.Run]--> Disconnecting \n");
	Disconnect();
	
	printf("[Server.Run]--> Ends \n");
	return 0;	
}		


