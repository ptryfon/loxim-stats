#ifndef __INDEX_MANAGER_H__
#define __INDEX_MANAGER_H__

#include "const.h"
#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
#include "Store/Store.h"
#include "IndexHandler.h"
#include "TransactionManager/Transaction.h"
#include "TransactionManager/Mutex.h"
#include "QueryParser/TreeNode.h"
#include "EntrySieve.h"
#include "VisibilityResolver.h"

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
using namespace Errors;
using namespace QExecutor;
using namespace TManager;
using namespace std; 

/**
 *	@author Piotr Turski (piotr.turski@students.mimuw.edu.pl)
 */
using namespace QParser;
namespace Indexes
{

	class IndexManager {
	
	public:
		typedef map<string, IndexHandler*> string2index;
		typedef map<string, string2index > indexesMap;
		typedef map<int, IndexHandler*> indexId_t;
		
	private:
	
		friend class EntrySieve;
		friend class QueryOptimizer;
		friend class Cleaner;
		friend class Tester;
		
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
		
		/** mapa potrzebna na czas wstawania systemu i wczytywania metadanych indeksow */
		indexId_t indexLid;
		
		static auto_ptr<SBQLConfig> config;
		
		auto_ptr<VisibilityResolver> resolver;
		
		//static string listFileName;
		//static string indexLockFile;
		
		static IndexManager* handle;	
		
		static ErrorConsole *ec;
		
		//Mutex DDLlock;// nie ma sensu robic czytelnikow i pisarzy - wylistowanie jest krotkie
		
		LogicalID *rootLid;
		
		/** nazwa pliku z metadanymi indeksow */
		string indexMetaFile;
		//string indexDataFile;
		
		TransactionManager *tm;
		
		/** nazwa roota przechowujacego dane o indeksach */
		string indexMetadata;
		
		/** nazwa pliku ze stronami indeksow */
		string indexDataFile;
		
		IndexManager();
		int instanceInit(bool cleanShutdown);
		
		/** splukuje wezly, zapisuje metadane, niszczy indeksy */
		int finalize();
		int getFileName(string paramName, string &value);
		int closeFiles(int errCode);
		//int fileOpen(string paramName, int &descriptor);
		
		/**
		 * wczytuje indeksy przy starcie bazy. fileName - nazwa root'a w ktorym sa metadane indeksow
		 */
		int loadIndexList(string &fileName);
		
		bool isRootIndexed(string name, indexesMap::iterator &it);
		bool isFieldIndexed(string &rootName, string &fieldName);
		bool isFieldIndexed(indexesMap::iterator &rootIterator, string fieldName);
		
		/** dodaje nowy indeks podczas wstawania systemu */
		int testAndAddIndex(string name, string rootName, string fieldName, int type, LogicalID* lid);
		
		/** uzywane do sprawdzenia przed dodaniem nowego indeksu */
		int isValuesUsed(string name, string rootName, string fieldName);
		
		/** dodaje nowy indeks do metadanych listy indeksow */
		IndexHandler* addIndex(string name, string rootName, string fieldName, LogicalID* lid);
		
		IndexHandler* getIndexFromField(const string &fieldName, indexesMap::iterator &it);
		
		int prepareToDrop(string indexName, string2index::iterator &itResult, LogicalID* &lid);
		//int dropIndex(string2index::iterator &it);
		
		int dropIndex(string indexName);
		
		int buildIndex(IndexHandler* ih, TransactionID* tid);
		//int addEmptyEntries(string2index &notUpdated, IndexHandler* toWhichIndex);
		
		//dodaje dataValue z roota
		int addDataValue(TransactionID* tid, indexesMap::iterator &it, DataValue* db_value, RootEntry* entry, IndexHandler* toWhichIndex);
		~IndexManager();
		
		/** odbudowuje indeksy na podstawie danych ze store'a */
		int rebuild();
		
		/** glowna metoda wywolywana w chwili usuwania roota w bazie 
		 *  zaklada ze jest wewnatrz lickow */
		int removeRoot(TransactionID* tid, ObjectPointer* op, indexesMap::iterator &it);
	public:
		
		/** synchronizacja dostepu do listy indeksow */
		RWUJSemaphore *indexListSem;
	
		//DDL - wywolywane z IndexNode
		int listIndex(QueryResult **result);
		//int createIndexSynchronized(string indexName, string indexedRootName, string indexedFieldName, Comparator* comp, QueryResult **result);
		//int dropIndexSynchronized(string indexName, QueryResult **result);
		int dropIndex(string indexName, QueryResult **result);
		int createIndex(string indexName, string indexedRootName, string indexedFieldName, Comparator* comp, QueryResult **result);
		
		//wywlywane z Transaction
		int modifyObject(TransactionID* tid, ObjectPointer* object, DataValue* value);
		int addRoot(TransactionID* tid, ObjectPointer* op, IndexHandler* toWhichIndex = NULL);
		int removeRoot(TransactionID* tid, ObjectPointer* op);
		void abort(tid_t id);
		void begin(tid_t id);
		void commit(tid_t id, ts_t commitTimestamp);
		
		/** wywolywane z obiektu transakcji podczas usuwanie obiektu.
		 *  sprawdza czy mozna usunac obiekt i wyrzuca go z indeksu */
		int deleteObject(ObjectPointer* object);
		
		//wywolywanie z QE
		int search(IndexSelectNode* node, Transaction* tr, QueryResult** result);
		
		//wywolywane z DBStoreManager. lid'om i objectPointer'om zawartym w rootach przypisuje nazwy rootow
		void setIndexMarker(LogicalID* lid, ObjectPointer* object);
		
		VisibilityResolver* getResolver();
		
		//wywolywane z Listener
		static int init(bool cleanShutdown);
		
		//zamyka caly modul indeksow 
		static int shutdown();
		
		static IndexManager* getHandle();
		
		friend class NonloggedDataSerializer;
	};

}

#endif /* __INDEX_MANAGER_H__ */
