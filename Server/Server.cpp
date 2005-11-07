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
#include "../QueryExecutor/QueryResult.h"
#include "../QueryExecutor/QueryExecutor.h"
#include "../Store/DBStoreManager.h"
#include "../TransactionManager/Transaction.h"

#include "Server.h"

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
using namespace QExecutor;
using namespace TManager;


Server::Server(int newSock)
{
	Sock = newSock;
	memset(messgBuff, 0, sizeof(messgBuff));
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
	
	SBQLConfig* config = new SBQLConfig();
	config->init();
	
	LogManager* logManager = new LogManager();
	logManager->init();
	
	DBStoreManager* storeManager = new DBStoreManager();
	storeManager->init(config, logManager);
	
	TransactionManager::getHandle()->init(storeManager);
	
	QueryParser *qPa = new QueryParser();
	QueryExecutor *qEx = new QueryExecutor();
		
	QueryTree *qTree = new QueryTree();
	QueryResult *qResult = new QueryResult();
	
	//Get string from client
	Receive(&size);
		
	qPa->parseIt ((string) messgBuff, qTree);
	qEx->queryResult(qTree, qResult);
		
	//memcpy(messgBuff, qResult, sizeof(qResult)); 
	//Send results to client
	Send(messgBuff, MESSGBUF_SIZE);
	
	//That's it, we no longer need this one..
	Disconnect();
	return 0;	
}		


