#include <iostream>
#include <cstdlib>
#include "DBStoreManager.h"
#include "DBDataValue.h"

#define LOGS

namespace Store
{
	StoreManager* StoreManager::theStore = NULL;

	DBStoreManager::DBStoreManager()
	{
		misc = new Misc();
		StoreManager::theStore = this;
		ec = new ErrorConsole("Store");
		config = new SBQLConfig("Store");
	}

	DBStoreManager::~DBStoreManager()
	{
		if(buffer) {
			buffer->stop();
			delete buffer;
			buffer = NULL;
		}
		if(map) {
			delete map;
			map = NULL;
		}
		if(pagemgr) {
			delete pagemgr;
			pagemgr = NULL;
		}
		if(ec) {
			delete ec;
			ec = NULL;
		}
		if(config) {
			delete config;
			config = NULL;
		}
		StoreManager::theStore = NULL;
	}

	int DBStoreManager::init(LogManager* log)
	{
		this->log = log;
		this->buffer = new Buffer(this);
		this->map = new Map(this);
		this->roots = new Roots(this);
		this->pagemgr = new PageManager(this);

		return 0;
	}

	int DBStoreManager::setTManager(TransactionManager* tm)
	{
		this->tm = tm;

		return 0;
	}

	int DBStoreManager::start()
	{
		int retval = 0;

		if ((retval = buffer->start()) != 0)
			return retval;

		return 0;
	}

	int DBStoreManager::stop()
	{
		int retval = 0;

		if ((retval = buffer->stop()) != 0)
			return retval;

		return 0;
	}

	SBQLConfig* DBStoreManager::getConfig()
	{
		return config;
	}

	LogManager* DBStoreManager::getLogManager()
	{
		return log;
	}

	TransactionManager* DBStoreManager::getTManager()
	{
		return tm;
	}

	Buffer* DBStoreManager::getBuffer()
	{
		return buffer;
	}

	Map* DBStoreManager::getMap()
	{
		return map;
	}
	
	Roots* DBStoreManager::getRoots()
	{
		return roots;
	}

	PageManager* DBStoreManager::getPageManager()
	{
		return pagemgr;
	}

	int DBStoreManager::getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object)
	{
		*ec << "Store::Manager::getObject started...";
		
		physical_id *p_id = NULL;
		if( (map->getPhysicalID(lid->toInteger(),&p_id)) == 2 ) return 2; //out of range
		cout << "file: " << p_id->file_id << ", page: " << p_id->page_id << ", off: " << p_id->offset <<endl;
		if((!p_id->file_id) && (!p_id->page_id) && (!p_id->offset)) return 2;
		if(p_id->offset > 1000) return 2;
		if(p_id->page_id > 1000) {
			*ec << "Store::Manager::getObject done: Object not found\n(brak ustalonego kodu bledu dla tej operacji, default -> return 2;";
			return 2;	
		}
		PagePointer *pPtr = buffer->getPagePointer(p_id->file_id, p_id->page_id);
		
		pPtr->aquire();
		
		*ec << "odpalam deserialize";
		int rval = PageManager::deserialize(pPtr, p_id->offset, object);
		
		pPtr->release();
		
		if(rval) {
			*ec << "Store::Manager::getObject failed";
			return -1;
		}
		ec->printf("Store::Manager::getObject done: %s\n", object->toString().c_str());
		return 0;	
	}

	int DBStoreManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object, LogicalID* p_lid)
	{
		*ec << "Store::Manager::createObject start...";
		
		LogicalID* lid;
		if(p_lid == NULL)
			lid = new DBLogicalID(map->createLogicalID());
		else
			lid = p_lid;
		unsigned log_id;
		int itid = tid==NULL ? -1 : tid->getId();
#ifdef LOGS
		// klony lida dla logow
		log->write(itid, lid->clone(), name, NULL, value->clone(), log_id);
#endif
		object = new DBObjectPointer(name, value, lid);

		Serialized sObj = object->serialize();
		sObj.info();

		int freepage = pagemgr->getFreePage(); // strona z wystaraczajaca iloscia miejsca na nowy obiekt
		//*ec << "Store::Manager::createObject freepage = " + freepage;
		cout << "Store::Manager::createObject freepage = " << freepage << endl;
		PagePointer* pPtr = buffer->getPagePointer(STORE_FILE_DEFAULT, freepage);

		pPtr->aquire();

		//cout << "przed:\n";
		//pagemgr->printPage(pPtr, 1024/16);
		int pidoffset;
		PageManager::insertObject(pPtr, sObj, &pidoffset, log_id);
		//cout << "po:\n";
		//pagemgr->printPage(pPtr, 1024/16);

		pagemgr->updateFreeMap(pPtr);
		
		//PageManager::printPage(pPtr, 1024/16);
		pPtr->release();
		
		physical_id pid;
		pid.page_id = freepage;
		pid.file_id = STORE_FILE_DEFAULT;
		pid.offset = pidoffset;
		map->setPhysicalID(lid->toInteger(), &pid);
		
		ec->printf("Store::Manager::createObject done: %s\n", object->toString().c_str());
		return 0;
	}

	int DBStoreManager::deleteObject(TransactionID* tid, ObjectPointer* object)
	{
		*ec << "Store::Manager::deleteObject start...";
		
		unsigned log_id;
		int itid = tid==NULL ? -1 : tid->getId();
#ifdef LOGS
		// klony dla logow
		log->write(itid, object->getLogicalID()->clone(), object->getName(), object->getValue()->clone(), NULL, log_id);
#endif
		physical_id *p_id = NULL;
		if( (map->getPhysicalID(object->getLogicalID()->toInteger(),&p_id)) == 2 ) return 2; //out of range
		cout << "file: " << p_id->file_id << ", page: " << p_id->page_id << ", off: " << p_id->offset <<endl;
		if((!p_id->file_id) && (!p_id->page_id) && (!p_id->offset)) return 2;
		PagePointer *pPtr = buffer->getPagePointer(p_id->file_id, p_id->page_id);

		pPtr->aquire();
		
		page_data *p = reinterpret_cast<page_data*>(pPtr->getPage());
		p->header.timestamp = log_id;		

// tr do konca

		int pos_table = p_id->offset;
		int end_of_object;
	
		if ( pos_table == 0 )
			end_of_object = STORE_PAGESIZE; // koniec strony
		else	//poczatek poprz obiektu
			end_of_object = p->object_offset[pos_table-1];
		
		// rozmiar usuwanego obiektu
		int object_size = end_of_object - p->object_offset[pos_table];
		
		char* page = pPtr->getPage();
		
		// przesuniecie obiektow na stronie
		int i;
		for(i = pos_table+1; i <= p->object_count-1; i++)
		{
		    char pom[STORE_PAGESIZE];
 		    
		    //rozmiar przesuwanego obiektu
		    int size = p->object_offset[i-1] - p->object_offset[i];
		    // poczatek przesuwanego obiektu
		    int start = p->object_offset[i];
		    
		    memmove(pom, page + start  , size);
		    memmove(page + start + object_size, pom, size);
		    
		};
		
		// uaktualnienie tablicy offsetow
		// oraz dodanie do starego offsetu rozmiaru usuwanego obiektu
		for(i = pos_table+1; i <= p->object_count-1; i++)
		    p->object_offset[i] = p->object_offset[i] + object_size;    

		// nagrobek 
		p->object_offset[pos_table] = -1;		

		// poinformowanie mapy o usunieciu obiektu
		memset(p_id, 0xFF, sizeof(p_id));
		map->setPhysicalID(object->getLogicalID()->toInteger(), p_id);

		// uaktualnienie info na stronie
		p->object_count--;
		p->free_space = p->free_space + object_size;

		pagemgr->updateFreeMap(pPtr);
		
		pPtr->release();

		ec->printf("Store::Manager::deleteObject done: %s\n", object->toString().c_str());
		
		return 0;
	}
	
	int DBStoreManager::modifyObject(TransactionID* tid, ObjectPointer*& object, DataValue* dv)
	{
		*ec << "Store::Manager::modifyObject start...";
		
		deleteObject(tid, object);
		ObjectPointer* newobj;
		createObject(tid, object->getName(), dv, newobj, object->getLogicalID());
		delete object;
		object = newobj;
		
		*ec << "Store::Manager::modifyObject done";
		return 0;
	}
	
	int DBStoreManager::replaceDV(ObjectPointer* object, DataValue* dv)
	{
		TransactionID t(99999);
		deleteObject(&t, object);
		ObjectPointer* newobj;
		createObject(&t, object->getName(), dv, newobj, object->getLogicalID());
		delete newobj;
		return 0;
	}

	int DBStoreManager::getRoots(TransactionID* tid, vector<ObjectPointer*>*& p_roots)
	{
		*ec << "Store::Manager::getRoots(ALL) begin..";

		int rval = getRoots(tid, "", p_roots);

		*ec << "Store::Manager::getRoots(ALL) done";
		return rval;
	}

	int DBStoreManager::getRoots(TransactionID* tid, string p_name, vector<ObjectPointer*>*& p_roots)
	{
		*ec << "Store::Manager::getRoots(BY NAME) begin..";
		p_roots = new vector<ObjectPointer*>(0);
		vector<int>* rvec;
		rvec = roots->getRoots();
		
		vector<int>::iterator obj_iter;
		for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++) {
			ObjectPointer* optr = NULL;
			LogicalID* lid = new DBLogicalID((*obj_iter));
			int rval = getObject(tid, lid, Store::Write, optr);
			if(optr || rval) {
				if( (optr->getName() == p_name) || (p_name == "") )
					p_roots->push_back(optr);
			} else
				return -1;
		}

		ec->printf("Store::Manager::getRoots(BY NAME) done: size=%d\n", p_roots->size());
		return 0;
	}

	int DBStoreManager::addRoot(TransactionID* tid, ObjectPointer* object)
	{
		*ec << "Store::Manager::addRoot begin..";
		int lid = object->getLogicalID()->toInteger();
		unsigned log_id;
		int itid = tid==NULL ? -1 : tid->getId();
#ifdef LOGS
		// klon dla logow
		log->addRoot(itid, object->getLogicalID()->clone(), log_id);
#endif
		roots->addRoot(lid);
		
		ec->printf("Store::Manager::addRoot done: %s\n", object->toString().c_str());
		return 0;
	}

	int DBStoreManager::removeRoot(TransactionID* tid, ObjectPointer* object)
	{
		*ec << "Store::Manager::removeRoot begin..";
		int lid = object->getLogicalID()->toInteger();
		unsigned log_id;
		int itid = tid==NULL ? -1 : tid->getId();
#ifdef LOGS
		// klon dla logow
		log->removeRoot(itid, object->getLogicalID()->clone(), log_id);
#endif
		roots->removeRoot(lid);
		
		ec->printf("Store::Manager::removeRoot done: %s\n", object->toString().c_str());
		return 0;
	}

	int DBStoreManager::abortTransaction(TransactionID* tid)
	{
		return 0;
	}

	int DBStoreManager::commitTransaction(TransactionID* tid)
	{
		return 0;
	}

	DataValue* DBStoreManager::createIntValue(int value)
	{
		return new DBDataValue(value);
	}

	DataValue* DBStoreManager::createDoubleValue(double value)
	{
		return new DBDataValue(value);
	}

	DataValue* DBStoreManager::createStringValue(string value)
	{
		return new DBDataValue(value);
	}

	DataValue* DBStoreManager::createPointerValue(LogicalID* value)
	{
		return new DBDataValue(value);
	}

	DataValue* DBStoreManager::createVectorValue(vector<LogicalID*>* value)
	{
		return new DBDataValue(value);
	}

	ObjectPointer* DBStoreManager::createObjectPointer(LogicalID* lid)
	{
		return new DBObjectPointer(lid);
	}

	ObjectPointer* DBStoreManager::createObjectPointer(LogicalID* lid, string name, DataValue* dv)
	{
		return new DBObjectPointer(name, dv, lid);
	}

	int DBStoreManager::logicalIDFromByteArray(unsigned char* buffer, LogicalID*& lid)
	{
		DBLogicalID* dlid;
		int rval = DBLogicalID::deserialize(buffer, dlid);
		lid = dlid;
		return rval;
	}

	int DBStoreManager::dataValueFromByteArray(unsigned char* buffer, DataValue*& value)
	{
		DBDataValue* dvalue;
		int rval = DBDataValue::deserialize(buffer, dvalue);
		value = dvalue;
		return rval;
	}

	DBPhysicalID* DBStoreManager::getPhysicalID(LogicalID* lid)
	{
		physical_id* pid;
		map->getPhysicalID(lid->toInteger(), &pid);
		return new DBPhysicalID(*pid);
	}
	
	int DBStoreManager::checkpoint(unsigned int& cid)
	{
#ifdef LOGS
		return getLogManager()->checkpoint(getTManager()->getTransactionsIds(), cid);
#else
		return 0;
#endif
	}
	
	int DBStoreManager::endCheckpoint(unsigned int& cid)
	{
#ifdef LOGS
		return getLogManager()->endCheckpoint(cid);
#else
		return 0;
#endif
	}
	
}
