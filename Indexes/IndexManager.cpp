#include "IndexManager.h"

//#include <stdio.h>

#define indexListParamName	"index_file_list"
#define indexDataParamName	"index_file_data"
#define indexMetadataParamName	"index_metadata"
#define loadIndexListErrMsg		"Cannot read index list\n"

#define descInitValue		-1
#define ROOT_DB_NAME	"root"
#define FIELD_DB_NAME	"field"

namespace Indexes
{
	
	IndexManager* IndexManager::handle = NULL;
	ErrorConsole* IndexManager::ec = NULL; 
	SBQLConfig IndexManager::config("IndexManager");
	string IndexManager::indexLockFile = "";
	string IndexManager::listFileName = "";
	
	
	int IndexManager::lockDesc = descInitValue;
	int IndexManager::listDesc = descInitValue;
	int IndexManager::dataDesc = descInitValue;

	int IndexManager::init(bool cleanShutdown) {
		ec = new ErrorConsole("IndexManager");
		
		handle = new IndexManager();
		int err = handle->instanceInit(cleanShutdown);
		if (err != 0) {
			shutdown();
			return err;
		}
		*ec << "started";		
		return 0;
	}
	
	int IndexManager::shutdown() {
		int err;
		if (handle == NULL) {
			return -1; //TODO IndexManager is already closed
		}
		
		*ec << "shutting down...";
		
		//delete lock
		
		err = handle->finalize();
		
		delete handle;
		handle = NULL;
		
		*ec << "closed";
		delete ec;
		return err;	
	}
	
	int IndexManager::fileOpen(string paramName, int &descriptor) {
		
		string value;
		
		int err;
		
		if ((err = getFileName(paramName, value))) {
			return err;
		}
  		
  		if( ( descriptor = ::open( value.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR ) ) < 0 ) {
			ec->printf("Cannot open file %s\n", value.c_str());
			return -1;
		}
		return 0;
	}
	
	int IndexManager::getFileName(string paramName, string &value) {
		
		int errCode;
		
		if( ( errCode = config.getString( paramName, value ) ) ) {
  			ec->printf("Cannot read %s from configuration file\n", paramName.c_str());
  			return errCode;
  		}
  		
  		return 0;
	}
	
	int IndexManager::closeFiles(int errCode) {
		int err;
		if (lockDesc != descInitValue) {
			if ((err = close(lockDesc))) {
				ec->printf("Error closing index lock file. Errorcode: %d\n", err);
			}
		}
		if (dataDesc != descInitValue) {
			if ((err = close(dataDesc))) {
				ec->printf("Error closing index data file. Errorcode: %d\n", err);
			}
		}
		if (listDesc != descInitValue) {
			if ((err = close(listDesc))) {
				ec->printf("Error closing index list file. Errorcode: %d\n", err);
			}
		}
		
		return errCode;
	}
	
	int IndexManager::instanceInit(bool cleanShutdown) {
		int errCode = 0;

  		SBQLConfig* config = new SBQLConfig( "IndexManager" );
  		tm = TransactionManager::getHandle();
  		
  		if( ( errCode = config->getString( indexMetadataParamName, indexMetadata ) ) ) {
  			ec->printf("Cannot read %s from configuration file\n", indexMetadataParamName);
  			return closeFiles(errCode);
  		}
		
		if ((errCode = DDLlock.init())) {
			return errCode;
		}
		
		if ((errCode = loadIndexList(indexMetadata))) {
			return errCode;
		}
		
		if (!cleanShutdown) {
			ec->printf("Shutdown wasn't clean. Rebuilding indexes...\n");
			//TODO przebudowac wszystkie indeksy, skasowac stare pliki
			ec->printf("Indexes rebuilding done\n");
		}
		
		return 0;
	}
	
	int IndexManager::loadIndexList(string &indexMetadata) {
		
		int err;
		 
		ec->printf("wczytuje indeksy\n");
		#ifdef INDEX_DEBUG_MODE
				ec->printf("INDEX::wczytuje indeksy z rootow: |%s|\n", indexMetadata.c_str());
		#endif
		
		Transaction *tr;
		if ((err = tm->createTransaction(tr))) {
			ec->printf(loadIndexListErrMsg);
			return err;
		}
		
		vector<ObjectPointer*>* indexes;
		
		// metadane indeksow przechowywane sa w root'cie o danej nazwie
		if ((err = tr->getRoots(indexMetadata, indexes))) {
			ec->printf(loadIndexListErrMsg);
			return err;
		}
		
		if (indexes->size() > 1) {
			ec->printf("uwaga - wiecej niz jeden obiekt zawiera metadane indeksow\n");
		}
		
		unsigned int i, j, k;
		ObjectPointer* optr;
		vector<LogicalID*> *nameLids, *fieldLids;
		string fieldNameValue;
		bool field, root;
		string indexName, rootName, fieldName;
		
		// dla kazdego root'a zawierajacego metadane wczytajmy indeksy ktore zawiera
		for (i = 0; i < indexes->size(); i++) {
			
			// obiekty w tych root'ach maja nazwe taka jak indeks i dwa obiekty typu string o nazwach "root" i "field"
			DataValue* value = indexes->at(i)->getValue();
			if (value->getType() != Store::Vector) {
				ec->printf("bledny wpis w metadanych indeksow. korzen nie jest obiektem zlozonym - pomijam\n");
				continue;
			}
			nameLids = value->getVector();
			// iterowanie po zawartosci root'a
			for (j = 0; j < nameLids->size(); j++) {
				if ((err = tr->getObjectPointer(nameLids->at(j), Store::Read, optr, false))) {
					ec->printf(loadIndexListErrMsg);
					return err;
				}
				// obiekt wewnatrz root'a = obiekt reprezentujacy indeks.jego nazwa = nazwa indeksu
				indexName = optr->getName();
				value = optr->getValue();
				// ten obiekt musi zawierac obiekty "root" i "field"
				if (value->getType() != Store::Vector) {
					ec->printf("bledny wpis w metadanych indeksow. obiekt %s nie zawiera informacji o indeksowanych polach - pomijam\n", indexName.c_str());
					continue;
				}
				fieldLids = value->getVector();
				if (fieldLids->size() != 2) {
					ec->printf("bledny wpis w metadanych indeksow. obiekt %s metadanych indeksow ma niewlasciwa liczbe pol - pomijam\n", indexName.c_str());
					continue;
				}
				field = root = false;
				// iteracja po zawartosci obiektu reprezentujacego indeks
				for (k = 0; k < fieldLids->size(); k++) {
					if ((err = tr->getObjectPointer(fieldLids->at(k), Store::Read, optr, false))) {
						ec->printf(loadIndexListErrMsg);
						return err;
					}
					value = optr->getValue();
					if (value->getType() != Store::String) {
						ec->printf("obiekt %s.%s metadanych indeksow ma niewlasciwy typ - pomijam\n", indexName.c_str(), optr->getName().c_str());
						continue;
					}
					if (optr->getName() == ROOT_DB_NAME) {
						// znaleziono nazwe indeksowanego root'a
						rootName = value->getString();
						root = true;
					} else if (optr->getName() == FIELD_DB_NAME) {
						// znaleziono nazwe indeksowanego pola
						fieldName = value->getString();
						field = true;
					} else {
						ec->printf("obiekt %s.%s metadanych indeksow ma niewlasciwa nazwe\n", indexName.c_str(), optr->getName().c_str());
						continue;
					}	
				}
				if (!(field && root)) {
					// nie znaleziono nazwy root'a albo nazwy pola 
					ec->printf("obiekt %s metadanych indeksow nie zawiera informacji o indeksowanych polach - pomijam\n", indexName.c_str());
					continue;
				}
				if ((err = testAndAddIndex(indexName, rootName, fieldName, nameLids->at(j)))) {
					//TODO moze da sie wypisac wlasciwa przyczyne bledu
					ec->printf("nie udalo sie utworzyc indeksu: %s on %s(%s). Powtorzona nazwa lub wskazane pole jest juz indeksowane. Kod bledu: %d\n", indexName.c_str(), rootName.c_str(), fieldName.c_str(), err);
					continue;
				}
			}
		} //petla po root'ach
		
		if (indexes->size() == 0) {
			//jesli nie ma zadnego root'a z metadanymi to jednego trzeba zrobic
			vector<LogicalID*>* vectorValue = new vector<LogicalID*>(0); 
			ObjectPointer *optr;
			if (
				(err = tr->createObject(indexMetadata, new DBDataValue(vectorValue), optr)) ||
				(err = tr->addRoot(optr))
			   ) {
				return err;
			}
			
			rootLid = optr->getLogicalID();
		} else {
			rootLid = indexes->at(0)->getLogicalID();
		}
		
		if ((err = tr->commit())) {
			ec->printf(loadIndexListErrMsg);
			return err;
		}
		
		ec->printf("zakonczono wczytywanie indeksow\n");
		return 0;
	}
	
	int IndexManager::finalize() {
		return 0;
	}
	
	IndexManager::IndexManager() {}
	IndexManager::~IndexManager() {}
	IndexManager* IndexManager::getHandle() {return handle;}
	
	int IndexManager::createIndexSynchronized(string indexName, string indexedRootName, string indexedFieldName, QueryResult **result) {
		int err, err1;
		
		if ((err = DDLlock.down())) return err;
		err = createIndex(indexName, indexedRootName, indexedFieldName, result);
		if ((err1 = DDLlock.up())) {
			ec->printf("fatal - failed to unlock mutex\n");
			return err1;	
		}
		
		return err;
	}
	
	int IndexManager::createIndex(string indexName, string indexedRootName, string indexedFieldName, QueryResult **result) 
	{
		int err = 0;
		if ((err = isValuesUsed(indexName, indexedRootName, indexedFieldName))) {
			return err;
		}
		
		*ec << "adding index to database";
		
		Transaction *tr;
		if ((err = tm->createTransaction(tr))) {
			ec->printf("Cannot create index\n");
			return err;
		}
	
		//TODO smartPtr
		vector<LogicalID*>* vectorValue = new vector<LogicalID*>(0); 
		
		ObjectPointer *optr;
		
		err = tr->createObject(ROOT_DB_NAME, new DBDataValue(indexedRootName), optr);
		if (err) {
			ec->printf("Cannot create index\n");
			return err;
		}
		
		vectorValue->push_back(optr->getLogicalID());
		
		err = tr->createObject(FIELD_DB_NAME, new DBDataValue(indexedFieldName), optr);
		if (err) {
			ec->printf("Cannot create index\n");
			return err;
		}
		
		vectorValue->push_back(optr->getLogicalID());
		
		err = tr->createObject(indexName, new DBDataValue(vectorValue), optr);
		if (err) {
			ec->printf("Cannot create index\n");
			return err;
		}
		
		//w optr jest cala metainformacja o indexie. trzeba ten obiekt podpiac pod roota
		
		ObjectPointer *rootOptr;
		
		if ((err = tr->getObjectPointer(rootLid, Store::Write, rootOptr, false))) {
			ec->printf("Cannot create index\n");
			return err;
		}
		rootOptr->getValue()->getVector()->push_back(optr->getLogicalID());
		
		//czy to zadziala? moze trzeba zrobic nowy obiekt
		DBDataValue *dbDataVal = new DBDataValue();
		dbDataVal->setVector(rootOptr->getValue()->getVector());
		
		err = tr->modifyObject(rootOptr, dbDataVal);
		if (err) {
			ec->printf("Cannot create index\n");
			return err;
		}
	
		if ((err = tr->commit())) {
			ec->printf("Cannot create index\n");
			return err;
		}
	
		addIndex(indexName, indexedRootName, indexedFieldName, optr->getLogicalID());
		
		*result = new QueryNothingResult();
		return 0;
	}

	int IndexManager::listIndex(QueryResult **result)
	{
		int err;
		if ((err = DDLlock.down())) {
			return err;
		}
		*ec << "listing indexes";
		string2index::const_iterator it;
		IndexHandler* ih;
		QueryBagResult *bagResult = new QueryBagResult();
		QueryStructResult *structResult;
		for (it = indexNames.begin(); it != indexNames.end(); ++it) {
			ih = it->second;
			ec->printf("index: %s on %s (%s)\n", ih->getName().c_str(), ih->getRoot().c_str(), ih->getField().c_str());
			structResult = new QueryStructResult();
			structResult->addResult(new QueryBinderResult("name", new QueryStringResult(ih->getName())));
			structResult->addResult(new QueryBinderResult("root", new QueryStringResult(ih->getRoot())));
			structResult->addResult(new QueryBinderResult("field", new QueryStringResult(ih->getField())));
			bagResult->addResult(structResult);
		}
		*result = bagResult;
		if ((err = DDLlock.up())) {
			ec->printf("fatal - failed to unlock mutex\n");
			return err;
		}
		return 0;	
	}

	int IndexManager::dropIndexSynchronized(string indexName, QueryResult **result) {
		int err, err1;
		
		if ((err = DDLlock.down())) return err;
		err = dropIndex(indexName, result);
		if ((err1 = DDLlock.up())) {
			ec->printf("fatal - failed to unlock mutex\n");
			return err1;	
		}
		
		return err;
	}

	int IndexManager::dropIndex(string indexName, QueryResult **result) {
		*ec << "dropping index";
		int err;
		
		string2index::iterator it;
		LogicalID* indexLid;
		
		if ((err = prepareToDrop(indexName, it, indexLid))) {
			return err;
		}
		
		Transaction *tr;
		ObjectPointer* optr;
		
		err = tm->createTransaction(tr);
		if (err) {
			ec->printf("Cannot drop index\n");
			return err;
		}
		
		err = tr->getObjectPointer(indexLid, Store::Write, optr, false);
		if (err) {
			ec->printf("Cannot drop index\n");
			return err;
		}
		
		err = tr->removeRoot(optr);
		if (err) {
			ec->printf("Cannot drop index\n");
			return err;
		}
		
		err = tr->deleteObject(optr);
		if (err) {
			ec->printf("Cannot drop index\n");
			return err;
		}
		
		err = tr->commit();
		if (err) {
			ec->printf("Cannot drop index\n");
			return err;
		}
			
		dropIndex(it);
		
		*result = new QueryNothingResult();
		return 0;
	}
}
