#include "IndexManager.h"

namespace Indexes
{
	
	//non thread-safe
	int IndexManager::addIndex(string name, string rootName, string fieldName) {
		//1. czy taka nazwa nie jest jeszcze uzywana
		
		if (indexNames.count(name) > 0) {
			//TODO nazwa juz uzywana
			ec->printf("INDEX::addIndex: nazwa %s jest juz uzywana\n", name.c_str());
			return -1;
		}
		
		//2. czy te pola nie sa jeszcze indeksowane
		
		if (isFieldIndexed(rootName, fieldName)) {
			//TODO
			ec->printf("INDEX::add Index: pole %s.%s jest juz indeksowane\n", rootName.c_str(), fieldName.c_str());
			return -1;
		}
		
		IndexHandler *ih = new IndexHandler(name, rootName, fieldName);
		//TODO wydajnosc - dodawac pare po jednokrotnym iterowaniu
		indexNames[name] = ih;
		indexes[rootName][fieldName] = ih;
		
		return 0;
	}
	
	//non thread-safe
	int IndexManager::removeIndex(string name) {
		return 0;
	}
	
	int IndexManager::modifyObject(TransactionID* tid, ObjectPointer* op, DataValue* value) {
			#ifdef INDEX_DEBUG_MODE
				ec->printf("INDEX::modyfikacja roota: %d, o nazwie: %s\n", op->getIsRoot(), op->getName().c_str());
			#endif
			
			return 0;
	}
	
	int IndexManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer* op, LogicalID* p_lid) {
		#ifdef INDEX_DEBUG_MODE
			ec->printf("INDEX::createObject. root: %d, o nazwie: %s, na wartosc: i lid: %d\n", op->getIsRoot(), op->getName().c_str(), p_lid->toInteger());
		#endif
		
		return 0;
	}
	
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
				//TODO czy indeksuje to pole?
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
	
	int IndexManager::dropIndex(string indexName) {
		string2index::iterator it = indexNames.find(indexName);
		if (it == indexNames.end()) {
			#ifdef INDEX_DEBUG_MODE
				ec->printf("nie ma indeksu o nazwie %s\n", indexName.c_str());
			#endif
			return -1;	
		}
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
