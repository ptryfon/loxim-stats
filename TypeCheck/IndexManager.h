#ifndef __INDEX_MANAGER_H__
#define __INDEX_MANAGER_H__

#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
#include "Store/Store.h"
#include "IndexHandler.h"
#include "TransactionManager/Transaction.h"
#include "TransactionManager/Mutex.h"
//#include "../QueryParser/IndexNode.h"

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
using namespace Errors;
using namespace QExecutor;
using namespace TManager;
using namespace std;

//#define INDEX_DEBUG_MODE 

namespace Indexes
{

	class IndexManager
	{
	private:
	
		typedef map<string, IndexHandler*> string2index;
		typedef map<string, string2index > indexesMap;
		
		/*
		 * potrzebne przy tworzeniu/usuwaniu indeksow
		 * sprawdzanie czy taka nazwa jeszcze nie jest uzywana
		 * sprawdzanie do jakiego indeksu sie odnosi
		 */ 
		 
		// indexName -> indexHandler
		string2index indexNames;
	
		/* 
		 * potrzebne przy dodawaniu/usuwaniu/modyfikacji rootow
		 * sprawdzanie czy ten root i to pole jest indeksowane (dodawanie rootow; tworzenie indeksu)
		 * odnajdywanie wlasciwego indeksu (dodawanie rootow)
		 */
	
		//rootName -> (fieldName -> indexHandler)
		indexesMap indexes;
	
		// decriptors
		static int lockDesc;
		static int listDesc;
		static int dataDesc;
		
		static SBQLConfig config;
		static string listFileName;
		static string indexLockFile;
		
		static IndexManager* handle;	
		
		static ErrorConsole *ec;
		
		Mutex DDLlock;
		LogicalID *rootLid;
		string indexListFile;
		string indexDataFile;
		
		TransactionManager *tm;
		string indexMetadata;
		
		IndexManager();
		int instanceInit(bool cleanShutdown);
		int finalize();
		int getFileName(string paramName, string &value);
		int closeFiles(int errCode);
		int fileOpen(string paramName, int &descriptor);
		int loadIndexList(string &fileName);
		
		bool isRootIndexed(string name, indexesMap::iterator &it);
		bool isFieldIndexed(string &rootName, string &fieldName);
		
		int testAndAddIndex(string name, string rootName, string fieldName, LogicalID* lid);
		int isValuesUsed(string name, string rootName, string fieldName);
		void addIndex(string name, string rootName, string fieldName, LogicalID* lid);
		
		IndexHandler* getIndexFromField(const string &fieldName, indexesMap::iterator &it);
		
		int prepareToDrop(string indexName, string2index::iterator &itResult, LogicalID* &lid);
		int dropIndex(string2index::iterator &it);
		int dropIndex(string indexName, QueryResult **result);
		int createIndex(string indexName, string indexedRootName, string indexedFieldName, QueryResult **result);
		//int removeIndex(string name);
		
		~IndexManager();
	
	public:
	
		//DDL - wywolywane z IndexNode
		int listIndex(QueryResult **result);
		int createIndexSynchronized(string indexName, string indexedRootName, string indexedFieldName, QueryResult **result);
		int dropIndexSynchronized(string indexName, QueryResult **result);
	
		//wywlywane z Transaction
		int modifyObject(TransactionID* tid, ObjectPointer* object, DataValue* value);
		int addRoot(TransactionID* tid, ObjectPointer* op);
		//int createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer* op, LogicalID* p_lid);
		
		//wywolywane z Listener
		static int init(bool cleanShutdown);
		static int shutdown();
		
		static IndexManager* getHandle();
	};

}

#endif /* __INDEX_MANAGER_H__ */
