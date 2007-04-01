#include "IndexManager.h"

//#include <stdio.h>

namespace Indexes
{
	
	IndexManager* IndexManager::handle = NULL;
	ErrorConsole* IndexManager::ec = NULL; 
	

	int IndexManager::init() {
		ec = new ErrorConsole("IndexManager");
		handle = new IndexManager();
		*ec << "started";		
		return 0;
	}
	
	int IndexManager::shutdown() {
		*ec << "shutting down...";
		delete handle;
		*ec << "closed";
		delete ec;
		return 0;	
	}
	
	IndexManager::IndexManager() {}
	
	IndexManager* IndexManager::getHandle() {
		return handle;
	}
	
	IndexManager::~IndexManager(){}
	
	/*int IndexManager::execute(IndexNode* node) {
		node->execute();
		return 0;
	}*/
	
	int IndexManager::createIndex(string indexName, string indexedRootName, string indexedFieldName, QueryResult **result) 
	{
		*ec << "creating index";
		*result = new QueryNothingResult();
		return 0;
	}

	int IndexManager::listIndex(QueryResult **result)
	{
		*ec << "listing indexes";
		*result = new QueryNothingResult();
		return 0;	
	}

	int IndexManager::dropIndex(string indexName, QueryResult **result) {
		*ec << "dropping index";
		*result = new QueryNothingResult();
		return 0;
	}
}
