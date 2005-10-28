
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <string>
#include "../Config/SBQLConfig.h"
#include "../Log/logs.h"
#include "../QueryExecutor/QueryExecutor.h"
#include "../QueryParser/QueryTree.h"
#include "../Store/DBStoreManager.h"



using namespace std;

int main(int argc, char *argv[]) {
	
	char input[50];
	cout << "SBQLCli ver 0.0.0.0.0.1 \n";
	cout << "> ";
	
	SBQLConfig* config = new SBQLConfig();
	config->init();
	
	LogManager* logManager = new LogManager();
	logManager->init("dupa.log");
	
	DBStoreManager* storeManager = new DBStoreManager();
	storeManager-> init(config, logManager);
	
	
		
	
	while (cin.getline(input,50)) {
		//parser(input);
		QueryTree* queryTree;
		Parser* parser = new Parser();
		//parser->init();
		parser->parseIt(new String(input), queryTree);
		
		
		QueryResult* queryResult;
		QueryExecutor* executor = new QueryExecutor();
		//executor->init();
		executor->queryResult(queryTree, queryResult);
		
//		ResultSet* result = con->execute(input);
//		cout << result->toString();
		cout << endl << "> ";
	 	cout << input << "\n";
	};
	return 0;	
}