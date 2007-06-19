#include "IndexManager.h"

namespace Indexes
{
	
	int IndexManager::testAndAddIndex(string name, string rootName, string fieldName, LogicalID* lid) {
		int err;
		if ((err = isValuesUsed(name, rootName, fieldName))) {
			return err;
		}
		addIndex(name, rootName, fieldName, lid);
		return 0;
	}
	
	//non thread-safe
	void IndexManager::addIndex(string name, string rootName, string fieldName, LogicalID* lid) {
		
		IndexHandler *ih = new IndexHandler(name, rootName, fieldName, lid);
		//TODO wydajnosc - dodawac pare po jednokrotnym iterowaniu
		indexNames[name] = ih;
		indexes[rootName][fieldName] = ih;
	}
	
	int IndexManager::isValuesUsed(string name, string rootName, string fieldName) {
		//1. czy taka nazwa nie jest jeszcze uzywana
		
		if (indexNames.count(name) > 0) {
			ec->printf("INDEX::addIndex: nazwa %s jest juz uzywana\n", name.c_str());
			return ErrIndexes | EIndexExists;
		}
		
		//2. czy te pola nie sa jeszcze indeksowane
		
		if (isFieldIndexed(rootName, fieldName)) {
			ec->printf("INDEX::add Index: pole %s.%s jest juz indeksowane\n", rootName.c_str(), fieldName.c_str());
			return ErrIndexes | EFieldIndexed;
		}
		
		return 0;
	}
	
	int IndexManager::modifyObject(TransactionID* tid, ObjectPointer* op, DataValue* value) {
			#ifdef INDEX_DEBUG_MODE
				ec->printf("INDEX::modyfikacja roota: %d, o nazwie: %s\n", op->getIsRoot(), op->getName().c_str());
			#endif
			
			return 0;
	}
	
	/* ta metoda prawdopodobnie nie bedzie potrzebna
	int IndexManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer* op, LogicalID* p_lid) {
		#ifdef INDEX_DEBUG_MODE
			ec->printf("INDEX::createObject. root: %d, o nazwie: %s, na wartosc: i lid: %d\n", op->getIsRoot(), op->getName().c_str(), p_lid->toInteger());
		#endif
		
		return 0;
	}
	*/
	
	int IndexManager::addRoot(TransactionID* tid, ObjectPointer* op) {

		indexesMap::iterator it;
		
		if (!isRootIndexed(op->getName(), it)) {
			#ifdef INDEX_DEBUG_MODE
				ec->printf("root o tej nazwie nie jest indexowany\n");
			#endif
			
			return 0;
		}
		
		#ifdef INDEX_DEBUG_MODE
			ec->printf("INDEX::dodanie roota: %d, o nazwie: %s i zawartosci: %s\n", op->getIsRoot(), op->getName().c_str(), op->toString().c_str());
		#endif
		
		DataValue* db_value = op->getValue();
		
		if (db_value->getType() != Store::Vector) {
			ec->printf("dodawany obiekt ma zly typ\n");
			//TODO co zrobic? zaindeksowac jako pusty?
			return -1;
		}
		
		LogicalID* lid;
		ObjectPointer* innerOp;
		vector<LogicalID*>::iterator pi;
		int errorNumber;
			for(pi=db_value->getVector()->begin(); pi!=db_value->getVector()->end(); pi++)
			{
				lid = *pi;
				errorNumber = LockManager::getHandle()->lock(lid, tid, Store::Read);
				
				errorNumber = Store::StoreManager::theStore->getObject(tid, lid, Store::Read, innerOp);
				
				#ifdef INDEX_DEBUG_MODE
					ec->printf("nowy root zawiera obiekt %s\n", innerOp->getName().c_str());
				#endif
				
				// czy indeksuje to pole?
				IndexHandler* ih;
				if (!(ih = getIndexFromField(innerOp->getName(), it))) {
					//pole nie jest indeksowane
					return 0;
				}
				#ifdef INDEX_DEBUG_MODE
					ec->printf("nowy root zawiera obiekt indeksowany %s\n", innerOp->getName().c_str());
				#endif
				//ih->addEntry();
			}
		
		return 0;
	}
	
	bool IndexManager::isRootIndexed(string name, indexesMap::iterator &it) {
		it = indexes.find(name);
		return it != indexes.end();
	}
	
	bool IndexManager::isFieldIndexed(string &rootName, string &fieldName) {
		indexesMap::iterator it;
		return isRootIndexed(rootName, it)  //czy root jest indeksowany
				&& ((it->second).count(fieldName) > 0); //i czy to pole jest indeksowane
		//return b;
		//return (isRootIndexed(rootName, it) && ((it->second).count(fieldName) > 0));
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
	
	int IndexManager::dropIndex(string2index::iterator &it) {
		
		IndexHandler* ih = it->second;
		indexNames.erase(it);
		#ifdef INDEX_DEBUG_MODE
			ec->printf("rozmiar indexNames po usunieciu: %d\n" ,indexNames.size());
		#endif
		indexesMap::iterator mapIt = indexes.find(ih->getRoot()); //na pewno bedzie znaleziony
		(mapIt->second).erase(ih->getField());
		#ifdef INDEX_DEBUG_MODE
			ec->printf("rozmiar mapy wewnetrznej po usunieciu: %d\n" ,(mapIt->second).size());
		#endif
		if ((mapIt->second).empty()) {
			indexes.erase(mapIt);
		}
		#ifdef INDEX_DEBUG_MODE
			ec->printf("rozmiar mapy zewnetrznej po usunieciu: %d\n" ,indexes.size());
		#endif
		delete ih; //TODO czy tu moze byc problem z usunieciem pliku?
		return 0;
	}
	
}
