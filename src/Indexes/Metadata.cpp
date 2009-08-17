#include <Indexes/IndexManager.h>
#include <Store/Store.h>
#include <Store/NamedItems.h>

#include <memory>

using namespace QExecutor;
using namespace Store;
namespace Indexes
{
	bool INDEX_DEBUG_MODE = false;

	int IndexManager::testAndAddIndex(string name, string rootName, string fieldName, int type, LogicalID* lid) {
		int err;
		if ((err = isValuesUsed(name, rootName, fieldName))) {
			return err;
		}
		IndexHandler* ih = addIndex(name, rootName, fieldName, lid);
		ih->load(Comparator::deserialize(type));
		//ih->load(nodeAddress_t rootAddr, int rootLevel);
		indexLid[lid->toInteger()] = ih;
		err = ih->changeState(IndexHandler::BUILDING, IndexHandler::READY);
		return err;
	}

	//non thread-safe
	IndexHandler* IndexManager::addIndex(string name, string rootName, string fieldName, LogicalID* lid) {

		IndexHandler *ih = new IndexHandler(name, rootName, fieldName, lid);
		indexNames[name] = ih;
		indexes[rootName][fieldName] = ih;
		return ih;
	}

	int IndexManager::isValuesUsed(string name, string rootName, string fieldName) {
		//1. czy taka nazwa nie jest jeszcze uzywana

		if (indexNames.count(name) > 0) {
			debug_printf(*ec, "INDEX::addIndex: nazwa %s jest juz uzywana\n", name.c_str());
			return ErrIndexes | EIndexExists;
		}

		//2. czy te pola nie sa jeszcze indeksowane

		if (isFieldIndexed(rootName, fieldName)) {
			debug_printf(*ec, "INDEX::add Index: pole %s.%s jest juz indeksowane\n", rootName.c_str(), fieldName.c_str());
			return ErrIndexes | EFieldIndexed;
		}

		return 0;
	}

	int IndexManager::removeRoot(TransactionID* tid, ObjectPointer* op, indexesMap::iterator &it) {
		int err;
		IndexHandler* ih;
		vector<LogicalID*> *content = op->getValue()->getVector();
		LogicalID* lid;
		ObjectPointer* optr;
		auto_ptr<RootEntry> entry;
		for(unsigned int i = 0; i < content->size(); i++) {
			lid = content->at(i);
			err = Store::StoreManager::theStore->getObject(tid, lid, Store::Read, optr);
			if (err) {
				return err;
			}

			if (INDEX_DEBUG_MODE) {
				printf("z root'a %s usuwane jest pole %s\n", op->getName().c_str(), optr->getName().c_str());
			}

			if (!(ih = getIndexFromField(optr->getName(), it))) {

				if (INDEX_DEBUG_MODE) {
					printf("pole %s nie jest indexowane\n", optr->getName().c_str());
				}

				//pole nie jest indeksowane
				continue;
			}
			entry.reset(new RootEntry(STORE_IXR_NULLVALUE, tid->getTimeStamp(), tid->getId(), op->getLogicalID()->toInteger())); //uzyty timestamp bo byl wczesniej

			//nic nie mozna usunac bo moze byc rollback
			ih->addEntry(optr->getValue(), entry.get());
		}
		return 0;
	}

	int IndexManager::removeRoot(TransactionID* tid, ObjectPointer* op) {
		indexesMap::iterator it;
		int err = 0;
		indexListSem->lock_write();
		if (isRootIndexed(op->getName(), it)) {
			err = removeRoot(tid, op, it);
		}
		indexListSem->unlock();

		return err;
	}

	int IndexManager::modifyObject(TransactionID* tid, ObjectPointer* op, DataValue* value) {
		int err = 0;
		auto_ptr<RootEntry> entry;

		if (INDEX_DEBUG_MODE) {
			printf("INDEX::modyfikacja roota: %d, o nazwie: %s\n", op->getIsRoot(), op->getName().c_str());
		}

		if (!op->getIsRoot()) {
			if (INDEX_DEBUG_MODE) {
				printf("modyfikowany obiekt nie jest rootem\n");
			}
			return 0;
		}

		indexesMap::iterator it;

		indexListSem->lock_write();

		if (!isRootIndexed(op->getName(), it)) {
			if (INDEX_DEBUG_MODE) {
				printf("root o nazwie %s nie jest indexowany\n", op->getName().c_str());
			}
			indexListSem->unlock();
			return err;
		}
		int err2 = removeRoot(tid, op, it);

		indexListSem->unlock();

		if (err2) {
			return err2;
		}

		entry.reset(new RootEntry(LogRecord::getIdSeq(), STORE_IXR_NULLVALUE, tid->getId(), op->getLogicalID()->toInteger()));// uzyte idSeq bo pozniej
		err = addDataValue(tid, it, value, entry.get(), NULL);

		return err;
	}

	int IndexManager::addDataValue(TransactionID* tid, indexesMap::iterator &it, DataValue* db_value, RootEntry* entry, IndexHandler* toWhichIndex) {
		string2index notUpdated = it->second; //liniowe od ilosci indeksow na tym root'cie
		string2index::iterator notUpdatedIt;
				//notUpdated zawiera wszystkie pola po ktorych musi byc indeksowanie

				if (db_value->getType() != Store::Vector) {
					debug_printf(*ec, "dodawany obiekt ma zly typ\n");
					return EBadValue | ErrIndexes;
					//ih->addEntry(tid, op);
					//return addEmptyEntries(notUpdated, toWhichIndex);
				}

				LogicalID* lid;
				ObjectPointer* innerOp;
				vector<LogicalID*>::iterator pi;
				int errorNumber;
				for(pi=db_value->getVector()->begin(); pi!=db_value->getVector()->end(); pi++)
				{
					//jesli okaze sie ze nie ma pola dla jakiegos indeksu to blad
					lid = *pi;
					errorNumber = LockManager::getHandle()->lock(lid, tid, Store::Read);

					errorNumber = Store::StoreManager::theStore->getObject(tid, lid, Store::Read, innerOp);

					if (INDEX_DEBUG_MODE) {
						printf("nowy root zawiera obiekt %s\n", innerOp->getName().c_str());
					}

					// czy indeksuje to pole?
					IndexHandler* ih;
					if (!(ih = getIndexFromField(innerOp->getName(), it))) {
						//pole nie jest indeksowane
						continue;
					}
					if (INDEX_DEBUG_MODE) {
						printf("nowy root zawiera obiekt indeksowany %s\n", innerOp->getName().c_str());
					}

					notUpdatedIt = notUpdated.find(innerOp->getName());
					if (notUpdatedIt == notUpdated.end()) {
						//pole o tej nazwie juz bylo, teraz probujemy uaktualniac je po raz drugi
						return EIndexedFieldDuplicated | ErrIndexes;
						//bedzie wykonany rollback
					}
					notUpdated.erase(notUpdatedIt);

					if ((toWhichIndex != NULL) && (toWhichIndex != ih)) {
						//w trakcie pracy bazy dodany zostal kolejny indeks i teraz tylko on jest uaktualniany. nie mozna dotknac zadnego innego
						continue;
					}
					bool buildingTransaction = (toWhichIndex != NULL);
					if ((errorNumber = ih->addEntry(innerOp->getValue(), entry, buildingTransaction))) {
						return errorNumber;
					}
				}

				if (notUpdated.size() == 0) {
					//ok, uaktualnilismy wszystkie indeksy na tym root'cie
					return 0;
				} else {
					//czesc indeksow nieuaktualniona. bedzie wykonany rollback
					return ENoIndexedField | ErrIndexes;
				}
	}

	int IndexManager::addRoot(TransactionID* tid, ObjectPointer* op, IndexHandler* toWhichIndex) {

		indexesMap::iterator it;

		if (!isRootIndexed(op->getName(), it)) {
			if (INDEX_DEBUG_MODE) {
				printf("root nazwie %s nie jest indexowany\n", op->getName().c_str());
			}

			return 0;
		}

		if (INDEX_DEBUG_MODE) {
			printf("INDEX::dodanie roota: %d, o nazwie: %s i zawartosci: %s\n", op->getIsRoot(), op->getName().c_str(), op->toString().c_str());
		}

		DataValue* db_value = op->getValue();

		auto_ptr<RootEntry> entry(new RootEntry(LogRecord::getIdSeq(), STORE_IXR_NULLVALUE, tid->getId(), op->getLogicalID()->toInteger()));
		return addDataValue(tid, it, db_value, entry.get(), toWhichIndex);

	}
	/*
	int IndexManager::addEmptyEntries(string2index &notUpdated, IndexHandler* toWhichIndex) {
		string2index::iterator it;
		IndexHandler* ih;
		for (it = notUpdated.begin(); it != notUpdated.end(); it++) {
			ih = it->second;
			if ((toWhichIndex != NULL) && (toWhichIndex != ih)) {
				//w trakcie pracy bazy dodany zostal kolejny indeks i teraz tylko on jest uaktualniany. nie mozna dotknac zadnego innego
				continue;
			}
		}
		return 0;
	}
	*/
	bool IndexManager::isRootIndexed(string name, indexesMap::iterator &it) {
		it = indexes.find(name);
		return it != indexes.end();
	}

	bool IndexManager::isFieldIndexed(string &rootName, string &fieldName) {
		indexesMap::iterator it;
		return isRootIndexed(rootName, it)  //czy root jest indeksowany
				&& isFieldIndexed(it, fieldName); //i czy to pole jest indeksowane
		//return b;
		//return (isRootIndexed(rootName, it) && ((it->second).count(fieldName) > 0));
	}

	bool IndexManager::isFieldIndexed(indexesMap::iterator &rootIterator, string fieldName) {
		return (rootIterator->second).count(fieldName) > 0;
	}

	IndexHandler* IndexManager::getIndexFromField(const string &fieldName, indexesMap::iterator &it) {
		string2index::const_iterator it2;
		it2 = (it->second).find(fieldName);
		if (it2 == (it->second).end()) {
			return NULL;
		} else {
			return it2->second;
		}
	}

	int IndexManager::prepareToDrop(string indexName, string2index::iterator &itResult, LogicalID* &lid) {
		string2index::iterator it = indexNames.find(indexName);
		if (it == indexNames.end()) {
			return ErrIndexes | ENoIndex;
		}
		itResult = it;
		lid = it->second->getLogicalID();
		return 0;
	}

	//index musi istniec
	int IndexManager::dropIndex(string indexName) {
		int err = 0;
		indexListSem->lock_write();

		string2index::iterator it = indexNames.find(indexName);
		IndexHandler* ih = it->second;
		indexNames.erase(it);
		if (INDEX_DEBUG_MODE) {
			debug_printf(*ec, "rozmiar indexNames po usunieciu: %d\n" ,indexNames.size());
		}
		indexesMap::iterator mapIt = indexes.find(ih->getRoot()); //na pewno bedzie znaleziony
		(mapIt->second).erase(ih->getField());
		if (INDEX_DEBUG_MODE) {
			printf("rozmiar mapy wewnetrznej po usunieciu: %ld\n" ,static_cast<long>((mapIt->second).size()));
		}
		if ((mapIt->second).empty()) {
			indexes.erase(mapIt);
		}
		if (INDEX_DEBUG_MODE) {
			printf("rozmiar mapy zewnetrznej po usunieciu: %ld\n" ,static_cast<long>(indexes.size()));
		}
		indexListSem->unlock();
		if ((err = ih->drop())) {
			return err;
		}
		delete ih;
		return 0;
	}

	//dodanie nowego indeksu w trakcie pracy bazy
	int IndexManager::buildIndex(IndexHandler* ih, TransactionID* tid) {

		int err;
		StoreManager* sm = dynamic_cast<StoreManager*>(Store::StoreManager::theStore);

		//czy tu powinien byc lockStore read?

		vector<Indexes::RootEntry*> *indexContent = new vector<Indexes::RootEntry*>(0);
		//nazwa, tid - nieistotne. maly timestamp = mniej operacji
		sm->getRoots()->getRoots(tid, "", indexContent);

		RootEntry* e;
		LogicalID* lid;
		vector<RootEntry*>::iterator it;
		ObjectPointer* optr;

		for(it=indexContent->begin(); it!=indexContent->end(); it++) {
			e = *it;
			lid = sm->createLID(e->logicalID);
			//err = LockManager::getHandle()->lock(lid, tid, Store::Read); //bez blokady bo to root

			if ((err = Store::StoreManager::theStore->getObject(tid, lid, Store::Read, optr))) {
				return err;
			}

			delete lid;
			// teraz optr wskazuje na roota
			if ((err = addRoot(tid, optr, ih))) { //ih jest zablokowany przed usunieciem
				return err; //czy tu unlock all?
			}
		}
		//czy tu store unlock?

		return 0;
	}

	int IndexManager::search(IndexSelectNode* node, Transaction* tr, QueryResult** result) {
		int err;

		//*ec << "search otrzymal node'a select index\n";
		//cout << node->putToString();

		indexListSem->lock_read();
		string2index::iterator it = indexNames.find(node->getIndexName());
		if (it == indexNames.end()) {
			indexListSem->unlock();
			
			return ErrIndexes | ENoIndex;
		}
		IndexHandler *ih = it->second;

		int err2 = ih->userEnter();

		indexListSem->unlock();

		if (err2) {
			return err2;
		}

		auto_ptr<EntrySieve> sieve(new EntrySieve(tr->getId(), ih->getId()));


		err = ih->search(node->getConstraints(), sieve.get());

		err2 = ih->userLeave();

		if (err){
			return err;
		}

		if (err2) {
			return err2;
		}

		sieveContent_t* vec = sieve->getResult();

		QueryBagResult *res = new QueryBagResult();
		//przekazanie zawartosci w stalym czasie
		(res->bag).swap(*vec);
		sieveContent_t v = res->bag;
		delete vec;

		*result = res;

		return 0;
	};

	void IndexManager::setIndexMarker(LogicalID* lid, ObjectPointer* object) {
		//jesli object->isRoot i vector to ustawic rootName wszystkim lid w wektorze
		if (object->getIsRoot() && (object->getValue()->getType() == Store::Vector)) {
			vector<LogicalID*> * vec = object->getValue()->getVector();
			for (unsigned int i = 0; i < vec->size(); i++) {
				vec->at(i)->setParentRoot(object->getName());
			}
			return;
		}

		//jesli lid jest w akims root'cie to oznaczyc tez objectPointer
		if (lid->getParentRoot() != "") {
			object->setParentRoot(lid->getParentRoot());
		}

	}

	int IndexManager::deleteObject(ObjectPointer* object) {

		indexesMap::iterator it;
		if (
				object->getParentRoot() != "" &&
				isRootIndexed(object->getParentRoot(), it) &&
				getIndexFromField(object->getName(), it)

			) {

			return ERemoveIndexed | ErrIndexes;
		}

		return 0;
	}

}
