
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <string>
#include "../Config/SBQLConfig.h"
#include "../Log/logs.h"
#include "../QueryParser/QueryParser.h"
#include "../QueryExecutor/QueryResult.h"
#include "../QueryExecutor/QueryExecutor.h"
#include "../Store/DBStoreManager.h"
#include "../TransactionManager/transaction.h"


using namespace std;
using namespace Config;
using namespace Logs;
using namespace Store;
using namespace QExecutor;

int main(int argc, char *argv[]) {
	
	string input;
	cout << "SBQLCli ver 0.0.0.0.0.1:) \n";
	cout << "> ";
	
	SBQLConfig* config = new SBQLConfig();
	config->init();
	
	LogManager* logManager = new LogManager();
	logManager->init();
	
	DBStoreManager* storeManager = new DBStoreManager();
	storeManager->init(config, logManager);
	
	TransactionManager* transactionManager = new TransactionManager();
	transactionManager->init(storeManager);
	
	while (getline(cin, input, '\n')) {
		
		QueryTree* queryTree;
		QueryParser* parser = new QueryParser();
		parser->parseIt(input, queryTree);

		QueryResult* queryResult;
		QueryExecutor* executor = new QueryExecutor();
		executor->init(transactionManager);
		executor->queryResult(queryTree, queryResult);

		cout << endl << "> ";
	};
	
	return 0;	
}
