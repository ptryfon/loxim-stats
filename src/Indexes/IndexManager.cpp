#include <Errors/ErrorConsole.h>
#include <Indexes/IndexManager.h>
#include <Indexes/QueryOptimizer.h>
#include <Indexes/VisibilityResolver.h>
#include <Indexes/NonloggedDataSerializer.h>
#include <Util/Concurrency.h>
//#include <stdio.h>

#define indexMetadataParamName	"index_metadata"
#define implicitIndexCallParamName	"implicit_index_call"
#define indexMetaFileParamName			"index_file_metadata"

#define loadIndexListErrMsg		"Cannot read index list\n"

#define descInitValue		-1
#define ROOT_DB_NAME	"root"
#define FIELD_DB_NAME	"field"
#define CMP_TYPE		"cmpType"


using namespace Util;
namespace Indexes
{

	IndexManager* IndexManager::handle = NULL;
	ErrorConsole* IndexManager::ec = NULL;
	auto_ptr<SBQLConfig> IndexManager::config(NULL);
	//auto_ptr< set<int> > IndexManager::rolledBack(NULL);
	//string IndexManager::indexLockFile = "";
	//string IndexManager::listFileName = "";


	int IndexManager::lockDesc = descInitValue;
	int IndexManager::listDesc = descInitValue;
	int IndexManager::dataDesc = descInitValue;

	int IndexManager::init(bool cleanShutdown) {
		ec = &ErrorConsole::get_instance(EC_INDEX_MANAGER);
		int err;
		config.reset(new SBQLConfig("Index"));
		//rolledBack.reset(new set<int>());

		handle = new IndexManager();
		if( ( err = config->getString( "index_file", handle->indexDataFile) ) ) {
	  		debug_printf(*ec, "Cannot read 'indexFile' from configuration file\n");
	  		return err;
  		}

		if ((err = IndexHandler::init(handle->indexDataFile))) {
			return err;
		}
		if ((err = handle->instanceInit(cleanShutdown))) {
			return err;
		}

		debug_print(*ec,  "started");
		return 0;
	}

	int IndexManager::shutdown() {
		int err = 0;
		if (handle == NULL) {
			return EIncorrectState | ErrIndexes;
		}

		debug_print(*ec,  "shutting down...");

		 //zapisanie metadanych
		if ((err = handle->finalize())) {
			return err;
		}
		if ((err = IndexHandler::destroy())) {
			return err;
		}

		delete handle;
		handle = NULL;
		debug_print(*ec,  "closed");
		return err;
	}
	/*
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
	*/
	int IndexManager::getFileName(string paramName, string &value) {

		int errCode;

		if( ( errCode = config->getString( paramName, value ) ) ) {
  			debug_printf(*ec, "Cannot read %s from configuration file\n", paramName.c_str());
  			return errCode;
  		}

  		return 0;
	}

	int IndexManager::closeFiles(int errCode) {
		int err;
		if (lockDesc != descInitValue) {
			if ((err = close(lockDesc))) {
				debug_printf(*ec, "Error closing index lock file. Errorcode: %d\n", err);
			}
		}
		if (dataDesc != descInitValue) {
			if ((err = close(dataDesc))) {
				debug_printf(*ec, "Error closing index data file. Errorcode: %d\n", err);
			}
		}
		if (listDesc != descInitValue) {
			if ((err = close(listDesc))) {
				debug_printf(*ec, "Error closing index list file. Errorcode: %d\n", err);
			}
		}

		return errCode;
	}

	int IndexManager::instanceInit(bool cleanShutdown) {
		int errCode = 0;

  		SBQLConfig* config = new SBQLConfig( "Index" );
  		tm = TransactionManager::getHandle();

  		if( ( errCode = config->getString( indexMetadataParamName, indexMetadata ) ) ) {
  			debug_printf(*ec, "Cannot read %s from configuration file\n", indexMetadataParamName);
  			return closeFiles(errCode);
  		}

  		if( ( errCode = config->getString( indexMetaFileParamName, indexMetaFile ) ) ) {
  			debug_printf(*ec, "Cannot read %s from configuration file\n", indexMetadataParamName);
  			return closeFiles(errCode);
  		}

  		bool implicitIndexCall;
  		if( ( errCode = config->getBool( implicitIndexCallParamName, implicitIndexCall ) ) ) {
			debug_printf(*ec, "Cannot read %s from configuration file\n", implicitIndexCallParamName);
			return closeFiles(errCode);
  		}
  		QueryOptimizer::setImplicitIndexCall(implicitIndexCall);

		indexListSem = new RWUJSemaphore();

		resolver.reset(new VisibilityResolver());

		if ((errCode = loadIndexList(indexMetadata))) {
			return errCode;
		}

		if (!cleanShutdown) {
			debug_printf(*ec, "Shutdown wasn't clean. Rebuilding indexes...\n");

			if ((errCode = rebuild(-1))) {
				return errCode;
			}

			debug_printf(*ec, "Indexes rebuilding done\n");
		} else {
			NonloggedDataSerializer* ds = new NonloggedDataSerializer();
			errCode = ds->deserialize(indexMetaFile);
			delete ds;
			if (errCode) {
				return errCode;
			}
		}

		return 0;
	}

	/*
	 * lista indeksow z comparatorami jest juz zaladowana
	 */
	int IndexManager::rebuild(int sessionId) {
		int err;
		indexLid.clear(); //metadane nie beda wczytywane - mapa nie bedzie potrzebna

		if ((err = unlink(indexMetaFile.c_str()))) {
			if (errno != ENOENT) {
				//int errc = errno;
				//printf("%s: %s\n",indexMetaFile.c_str(), strerror(errc));
				return errno | ErrIndexes;
			}
		}

		if ((err = truncate(indexDataFile.c_str(), 0))) {
			return errno | ErrIndexes;
		}

		string2index::iterator it;
		for (it = indexNames.begin(); it != indexNames.end(); it++) {
			if ((err = it->second->reset())) {
				return err;
			}
		}

		Transaction *tr;
		if ((err = tm->createTransaction(sessionId, tr))) {
			return err;
		}

		if ((err = buildIndex(NULL, tr->getId()))) {
			return err;
		}

		if ((err = tr->commit())) {
			return err;
		}
		return 0;
	}

	int IndexManager::loadIndexList(string &indexMetadata) {

		int err;

		debug_printf(*ec, "loading indexes\n");

		Transaction *tr;
		if ((err = tm->createTransaction(-1, tr))) {
			debug_printf(*ec, loadIndexListErrMsg);
			return err;
		}

		vector<ObjectPointer*>* indexes;

		// metadane indeksow przechowywane sa w root'cie o danej nazwie
		if ((err = tr->getRoots(indexMetadata, indexes))) {
			debug_printf(*ec, loadIndexListErrMsg);
			return err;
		}

		if (indexes->size() > 1) {
			debug_printf(*ec, "wiecej niz jeden obiekt zawiera metadane indeksow\n");
			return EMetaIncorrect | ErrIndexes;
		}

		unsigned int j, k;
		int compType;
		ObjectPointer* optr, *indexOptr;
		vector<LogicalID*> *nameLids, *fieldLids;
		string fieldNameValue;
		bool field, root, type;
		string indexName, rootName, fieldName;

		if (indexes->size() == 1) {
			// dla kazdego root'a zawierajacego metadane wczytajmy indeksy ktore zawiera
			indexOptr = indexes->at(0);

			// obiekty w tych root'ach maja nazwe taka jak indeks i dwa obiekty typu string o nazwach "root" i "field"
			DataValue* value = indexOptr->getValue();
			if (value->getType() != Store::Vector) {
				debug_printf(*ec, "bledny wpis w metadanych indeksow. korzen nie jest obiektem zlozonym\n");
				return EMetaIncorrect | ErrIndexes;
			}
			nameLids = value->getVector();
			// iterowanie po zawartosci root'a
			for (j = 0; j < nameLids->size(); j++) {
				if ((err = tr->getObjectPointer(nameLids->at(j), Store::Read, optr, false))) {
					debug_printf(*ec, loadIndexListErrMsg);
					return err;
				}
				// obiekt wewnatrz root'a = obiekt reprezentujacy indeks.jego nazwa = nazwa indeksu
				indexName = optr->getName();
				value = optr->getValue();
				// ten obiekt musi zawierac obiekty "root" i "field"
				if (value->getType() != Store::Vector) {
					debug_printf(*ec, "bledny wpis w metadanych indeksow. obiekt %s nie zawiera informacji o indeksowanych polach - pomijam\n", indexName.c_str());
					continue;
				}
				fieldLids = value->getVector();
				if (fieldLids->size() != 3) {
					debug_printf(*ec, "bledny wpis w metadanych indeksow. obiekt %s metadanych indeksow ma niewlasciwa liczbe pol - pomijam\n", indexName.c_str());
					continue;
				}
				field = root = type = false;
				// iteracja po zawartosci obiektu reprezentujacego indeks
				for (k = 0; k < fieldLids->size(); k++) {
					if ((err = tr->getObjectPointer(fieldLids->at(k), Store::Read, optr, false))) {
						debug_printf(*ec, loadIndexListErrMsg);
						return err;
					}
					value = optr->getValue();
					/*
					if (value->getType() != Store::String) {
						ec->printf("obiekt %s.%s metadanych indeksow ma niewlasciwy typ - pomijam\n", indexName.c_str(), optr->getName().c_str());
						continue;
					}
					*/
					if (optr->getName() == ROOT_DB_NAME) {
						// znaleziono nazwe indeksowanego root'a
						rootName = value->getString();
						root = true;
					} else if (optr->getName() == FIELD_DB_NAME) {
						// znaleziono nazwe indeksowanego pola
						fieldName = value->getString();
						field = true;
					} else if (optr->getName() == CMP_TYPE) {
						compType = value->getInt();
						type = true;
					} else {
						debug_printf(*ec, "obiekt %s.%s metadanych indeksow ma niewlasciwa nazwe\n", indexName.c_str(), optr->getName().c_str());
						continue;
					}
				}
				if (!(field && root && type)) {
					// nie znaleziono nazwy root'a albo nazwy pola
					debug_printf(*ec, "obiekt %s metadanych indeksow nie zawiera informacji o indeksowanych polach\n", indexName.c_str());
					return EMetaIncorrect | ErrIndexes;
				}
				if ((err = testAndAddIndex(indexName, rootName, fieldName, compType, nameLids->at(j)))) {
					debug_printf(*ec, "nie udalo sie utworzyc indeksu: %s on %s(%s). Powtorzona nazwa lub wskazane pole jest juz indeksowane. Kod bledu: %d\n", indexName.c_str(), rootName.c_str(), fieldName.c_str(), err);
					return err;
				}
			}

		}
		if (indexes->size() == 0) {
			//jesli nie ma zadnego root'a z metadanymi to jednego trzeba zrobic
			vector<LogicalID*>* vectorValue = new vector<LogicalID*>(0);
			ObjectPointer *optr;
			StoreManager *sm = StoreManager::theStore;
			if (
				(err = tr->createObject(indexMetadata, sm->createVectorValue(vectorValue), optr)) ||
				(err = tr->addRoot(optr))
			   ) {
				return err;
			}

			rootLid = optr->getLogicalID();
		} else {
			rootLid = indexes->at(0)->getLogicalID();
		}

		if ((err = tr->commit())) {
			debug_printf(*ec, loadIndexListErrMsg);
			return err;
		}

		debug_printf(*ec, "zakonczono wczytywanie indeksow\n");
		return 0;
	}

	int IndexManager::finalize() {
		//tutaj nie powinno byc juz zadnej aktywnej transakcji
		int err = 0;
		if ((err = IndexHandler::flushAll())) {
			return err;
		}
		NonloggedDataSerializer* ds = new NonloggedDataSerializer();
		if ((err = ds->serialize(indexMetaFile))) {
			return err;
		}
		delete ds;

		//usuwanie wszystkich indeksow
		string2index::iterator it;
		for (it = indexNames.begin(); it != indexNames.end(); it++) {
			delete it->second;
		}

		delete indexListSem;

		return 0;
	}

	IndexManager::IndexManager() {}
	IndexManager::~IndexManager() {}
	IndexManager* IndexManager::getHandle() {return handle;}

	VisibilityResolver* IndexManager::getResolver() {return resolver.get();}

	/**
	 * tworzy indeks po wydaniu polecenia z palca
	 */
	int IndexManager::createIndex(int sessionId, string indexName, string indexedRootName, string indexedFieldName, Comparator* comp, QueryResult **result)
	{
		int err = 0;
		// czy taki indeks mozna utworzyc
		indexListSem->lock_write();

		IndexHandler *ih = NULL;

		int err2 = 0;
		if (!(err2 = isValuesUsed(indexName, indexedRootName, indexedFieldName))) {
			// dodaje wpis do metadanych. lid bedzie podany pozniej
			ih= addIndex(indexName, indexedRootName, indexedFieldName, NULL);
		}

		indexListSem->unlock();
		if (err2) {
			return err2;
		}

		debug_print(*ec,  "adding index to database");
		Transaction *tr;
		if ((err = tm->createTransaction(sessionId, tr))) {
			debug_printf(*ec, "Cannot create index\n");
			return err;
		}

		vector<LogicalID*>* vectorValue = new vector<LogicalID*>(0);

		ObjectPointer *optr;
		StoreManager *sm = StoreManager::theStore;
		err = tr->createObject(ROOT_DB_NAME, sm->createStringValue(indexedRootName), optr);
		if (err) {
			debug_printf(*ec, "Cannot create index\n");
			if ((err2 = tr->abort())) {
				return err2;
			}
			return err;
		}

		vectorValue->push_back(optr->getLogicalID());

		err = tr->createObject(FIELD_DB_NAME, sm->createStringValue(indexedFieldName), optr);
		if (err) {
			debug_printf(*ec, "Cannot create index\n");
			if ((err2 = tr->abort())) {
				return err2;
			}
			return err;
		}

		vectorValue->push_back(optr->getLogicalID());

		err = tr->createObject(CMP_TYPE, sm->createIntValue(comp->serialize()), optr);
		if (err) {
			debug_printf(*ec, "Cannot create index\n");
			if ((err2 = tr->abort())) {
				return err2;
			}
			return err;
		}

		vectorValue->push_back(optr->getLogicalID());

		err = tr->createObject(indexName, sm->createVectorValue(vectorValue), optr);
		if (err) {
			debug_printf(*ec, "Cannot create index\n");
			if ((err2 = tr->abort())) {
				return err2;
			}
			return err;
		}
		//w optr jest cala metainformacja o indexie. trzeba ten obiekt podpiac pod roota

		ObjectPointer *rootOptr;

		if ((err = tr->getObjectPointer(rootLid, Store::Write, rootOptr, false))) {
			debug_printf(*ec, "Cannot create index\n");
			if ((err2 = tr->abort())) {
				return err2;
			}
			return err;
		}
		rootOptr->getValue()->getVector()->push_back(optr->getLogicalID());

		//czy trzeba robic nowy obiekt?
		DataValue *dbDataVal = sm->createVectorValue(
			rootOptr->getValue()->getVector());

		LogicalID* lid = optr->getLogicalID()->clone();

		ih->setLid(lid);

		err = tr->modifyObject(rootOptr, dbDataVal);
		if (err) {
			debug_printf(*ec, "Cannot create index\n");
			if ((err2 = tr->abort())) {
				return err2;
			}
			return err;
		}

		//fizyczne utworzenie indeksu
		if ((err = ih->createNew(comp))) {
			return err;
		}

		// buduje indeks w trakcie pracy bazy
		if ((err = buildIndex(ih, tr->getId()))) {
			//jesli byly niewlasciwe obiekty do indeksowania to nie jest blad krytyczny, usunac indeks z mapy i zwolnic wezly
			if ((err2 = tr->abort())) {
				return err2;
			}
			if ((err2 = dropIndex(indexName))) {
				debug_printf(*ec, "Cannot undo 'create index'\n");
				//fatal
				return err2;
			}
			return err;
		}

		if ((err = tr->commit())) {
			debug_printf(*ec, "Cannot create index\n");
			if (dropIndex(indexName)) {
				debug_printf(*ec, "Cannot undo 'create index'\n");
				//fatal
				exit(1);
			}

			return err;
		}
		if ((err = ih->changeState(IndexHandler::BUILDING, IndexHandler::READY))) {
			return err;
		}

		*result = new QueryNothingResult();
		return 0;
	}

	int IndexManager::listIndex(QueryResult **result)
	{
		int err;
		indexListSem->lock_read();
		debug_print(*ec,  "listing indexes");
		string2index::const_iterator it;
		IndexHandler* ih;
		QueryBagResult *bagResult = new QueryBagResult();
		QueryStructResult *structResult;
		for (it = indexNames.begin(); it != indexNames.end(); ++it) {
			ih = it->second;
			debug_printf(*ec, "index: %s on %s (%s)\n", ih->getName().c_str(), ih->getRoot().c_str(), ih->getField().c_str());
			structResult = new QueryStructResult();
			structResult->addResult(new QueryBinderResult("name", new QueryStringResult(ih->getName())));
			structResult->addResult(new QueryBinderResult("type", new QueryStringResult(ih->getType())));
			structResult->addResult(new QueryBinderResult("root", new QueryStringResult(ih->getRoot())));
			structResult->addResult(new QueryBinderResult("field", new QueryStringResult(ih->getField())));
			bagResult->addResult(structResult);
		}
		*result = bagResult;
		indexListSem->unlock();
		return 0;
	}

	int IndexManager::dropIndex(int sessionId, string indexName, QueryResult **result) {
		debug_print(*ec,  "dropping index");
		int err;

		string2index::iterator it;
		LogicalID* indexLid;
		indexListSem->lock_read();
		int err2 = prepareToDrop(indexName, it, indexLid);

		if (err2 == 0) {
			err2 = it->second->changeState(IndexHandler::READY, IndexHandler::DROPPING);
		}

		indexListSem->unlock();

		if (err2) {
			return err2;
		}
		Transaction *tr;
		ObjectPointer* optr;

		err = tm->createTransaction(sessionId, tr);
		if (err) {
			debug_printf(*ec, "Cannot drop index\n");
			return err;
		}

		err = tr->getObjectPointer(indexLid, Store::Write, optr, false);
		if (err) {
			debug_printf(*ec, "Cannot drop index\n");
			return err;
		}

		err = tr->removeRoot(optr);
		if (err) {
			debug_printf(*ec, "Cannot drop index\n");
			return err;
		}

		err = tr->deleteObject(optr);
		if (err) {
			debug_printf(*ec, "Cannot drop index\n");
			return err;
		}

		err = tr->commit();
		if (err) {
			debug_printf(*ec, "Cannot drop index\n");
			return err;
		}

		dropIndex(indexName);

		*result = new QueryNothingResult();
		return 0;
	}

	void IndexManager::abort(tid_t id) {
		resolver->abort(id);
		//rolledBack.insert(id);
	}

	void IndexManager::begin(tid_t id) {
		resolver->begin(id);
	}

	void IndexManager::commit(tid_t tid, ts_t commitTimestamp) {
		resolver->commit(tid, commitTimestamp);
	}
}
