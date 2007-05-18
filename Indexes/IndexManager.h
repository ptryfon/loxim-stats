#ifndef __INDEX_MANAGER_H__
#define __INDEX_MANAGER_H__

#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
//#include "../QueryParser/IndexNode.h"

#include <string>
#include <fstream>
#include <iostream>
using namespace Errors;
using namespace QExecutor;
using namespace std;

//namespace QParser{ 

namespace Indexes
{
	//class IndexNode;

	class IndexManager
	{
	private:
	
		// decriptors
		static int lockDesc;
		static int listDesc;
		static int dataDesc;
		
		static SBQLConfig config;
		static string listFileName;
		static string indexLockFile;
		
		static IndexManager* handle;	
		IndexManager();
		static ErrorConsole *ec;
		string indexListFile;
		string indexDataFile;
		int instanceInit(bool cleanShutdown);
		int finalize();
		int getFileName(string paramName, string &value);
		int closeFiles(int errCode);
		int fileOpen(string paramName, int &descriptor);
		int loadIndexList(string &fileName);
	
	public:
	
		static int init(bool cleanShutdown);
		int createIndex(string indexName, string indexedRootName, string indexedFieldName, QueryResult **result);
		int listIndex(QueryResult **result);
		int dropIndex(string indexName, QueryResult **result);
		//void list
		virtual ~IndexManager();
		static IndexManager* getHandle();
		static int shutdown();
		//int execute(IndexNode* node);
	};

}

#endif /* __INDEX_MANAGER_H__ */
