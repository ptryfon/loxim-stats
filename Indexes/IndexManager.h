#ifndef __INDEX_MANAGER_H__
#define __INDEX_MANAGER_H__

#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
//#include "../QueryParser/IndexNode.h"

#include <string>
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
	
		static IndexManager* handle;	
		IndexManager();
		static ErrorConsole *ec;
	
	public:
	
		static int init();
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
