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
		ec = new ErrorConsole("Store");
#ifdef DEBUG_MODE
		*ec << "Store::StoreManager::Constructor begin";
#endif
		misc = new Misc();
		StoreManager::theStore = this;
		config = new SBQLConfig("Store");
#ifdef DEBUG_MODE
		*ec << "Store::StoreManager::Constructor done";
#endif
	}

	DBStoreManager::~DBStoreManager()
	{
#ifdef DEBUG_MODE
		*ec << "Store::StoreManager::Destructor begin";
#endif
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
#ifdef DEBUG_MODE
		*ec << "Store::StoreManager::Destructor done";
#endif
	}

	int DBStoreManager::init(LogManager* log)
	{
#ifdef DEBUG_MODE
		*ec << "Store::StoreManager::init begin";
#endif
		this->log = log;
		this->buffer = new Buffer(this);
		this->map = new Map();
		this->roots = new NamedRoots();
		this->views = new Views();
		this->pagemgr = new PageManager(this);

		this->roots->init(this->buffer, this->log);
		this->views->init(this->buffer, this->log);
		this->map->init(this->buffer, this->log);

#ifdef DEBUG_MODE
		*ec << "Store::StoreManager::init done";
#endif
		return 0;
	}

	int DBStoreManager::setTManager(TransactionManager* tm)
	{
#ifdef DEBUG_MODE
		*ec << "Store::StoreManager::setTManager";
#endif
		this->tm = tm;

		return 0;
	}

	int DBStoreManager::start()
	{
#ifdef DEBUG_MODE
		*ec << "Store::StoreManager::start";
#endif
		int retval = 0;

		if ((retval = buffer->start()) != 0)
			return retval;

		return 0;
	}

	int DBStoreManager::stop()
	{
#ifdef DEBUG_MODE
		*ec << "Store::StoreManager::stop";
#endif
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
	
	NamedRoots* DBStoreManager::getRoots()
	{
		return roots;
	}

	Views* DBStoreManager::getViews()
	{
		return views;
	}

	PageManager* DBStoreManager::getPageManager()
	{
		return pagemgr;
	}

	int DBStoreManager::getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getObject (LID=%d) started...", lid->toInteger());
#endif
		physical_id *p_id = NULL;
		if( (map->getPhysicalID(lid->toInteger(),&p_id)) == 2 ) return 2; //out of range
		cout << "file: " << p_id->file_id << ", page: " << p_id->page_id << ", off: " << p_id->offset <<endl;

		if (map->equal(p_id, map->RIP))
		{
			*ec << "Store::Manager::getObject failed: Object not found\n(brak ustalonego kodu bledu dla tej operacji, default -> return 2;";
			return 2;	
		}

		PagePointer *pPtr = buffer->getPagePointer(p_id->file_id, p_id->page_id);
		
		pPtr->aquire();
		
		int rval = PageManager::deserialize(pPtr, p_id->offset, object);
		
		pPtr->release(0);
		
		if(rval) {
			*ec << "Store::Manager::getObject failed";
			return -1;
		}
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getObject done: %s\n", object->toString().c_str());
#endif
		return 0;	
	}

	int DBStoreManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object, LogicalID* p_lid)
	{
		return createObject(tid, name, value, object, p_lid, false);
	}

	int DBStoreManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object, LogicalID* p_lid, bool isRoot)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::createObject start...";
#endif		
		LogicalID* lid;
		if(p_lid == NULL)
			lid = new DBLogicalID(map->createLogicalID());
		else
			lid = p_lid;
		unsigned log_id;
		int itid = tid==NULL ? -1 : tid->getId();
#ifdef LOGS
		// klony lida dla logow
		log->write(itid, lid->clone(), name, NULL, value->clone(), log_id/*, p_lid?false:true*/);
#endif
		object = new DBObjectPointer(name, value, lid);
		object->setIsRoot(isRoot);

		Serialized sObj = object->serialize();
		sObj.info();

		int freepage = pagemgr->getFreePage(sObj.size); // strona z wystaraczajaca iloscia miejsca na nowy obiekt
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::createObject freepage = %d\n", freepage);
#endif
		PagePointer* pPtr = buffer->getPagePointer(STORE_FILE_DEFAULT, freepage);

		pPtr->aquire();

		int pidoffset;
		PageManager::insertObject(pPtr, sObj, &pidoffset, log_id);

		pagemgr->updateFreeMap(pPtr);
		
		pPtr->release(1);
		
		physical_id pid;
		pid.page_id = freepage;
		pid.file_id = STORE_FILE_DEFAULT;
		pid.offset = pidoffset;
		map->setPhysicalID(lid->toInteger(), &pid);
		
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::createObject done: %s\n", object->toString().c_str());
#endif
		return 0;
	}


	int DBStoreManager::deleteObject(TransactionID* tid, ObjectPointer* object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::deleteObject start...";
#endif
		unsigned log_id;
		int itid = tid==NULL ? -1 : tid->getId();
#ifdef LOGS
		// klony dla logow
		log->write(itid, object->getLogicalID()->clone(), object->getName(), object->getValue()->clone(), NULL, log_id);
#endif
		physical_id *p_id = NULL;
		if( (map->getPhysicalID(object->getLogicalID()->toInteger(),&p_id)) == 2 ) {
			ec->printf("Store::Manager::deleteObject failed: LID=%d out of range\n(brak ustalonego kodu bledu dla tej operacji, default -> return 2;\n", object->getLogicalID()->toInteger());
			return 2; //out of range
		}
#ifdef DEBUG_MODE
		ec->printf("file: %d, page: %d, offset: %d\n", p_id->file_id, p_id->page_id, p_id->offset);
#endif
		if((!p_id->file_id) && (!p_id->page_id) && (!p_id->offset)) return 2;
		if( p_id->offset > STORE_PAGESIZE/4 ) {
			*ec << "Store::Manager::deleteObject failed: invalid offset";
			return 3;
		}
		PagePointer *pPtr = buffer->getPagePointer(p_id->file_id, p_id->page_id);

		pPtr->aquire();
		
		page_data *p = reinterpret_cast<page_data*>(pPtr->getPage());
		p->header.timestamp = log_id;		

		int ooff = p_id->offset;
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
		// md243003: nie moze byc nagrobka ze wzgledu na uzywane
		// powyzej algorytym wykorzystujace sasiadujace offsety
		p->object_offset[pos_table] += object_size; // = -1;		

		// poinformowanie mapy o usunieciu obiektu
		map->setPhysicalID(object->getLogicalID()->toInteger(), map->RIP);

		// uaktualnienie info na stronie
		if(p->object_count-1 == ooff) {
			p->object_count--;
			p->free_space = p->free_space + object_size + sizeof(int);
		} else
			p->free_space = p->free_space + object_size;

		// wyczyszczenie koncowki pustych obiektow
		// pusty obiekt to taki ktorego wskaznik jest rowny
		// wskaznikowi poprzednika
		for(i = p->object_count-1; i > 0; i--)
			if( p->object_offset[i] >= p->object_offset[i-1] )
				if( p->object_offset[i] > p->object_offset[i-1] ) {
					*ec << "Store::Manager::deleteObject corrupted page offsets detected";
					break;
				}
				else {
					p->object_count--;
					p->free_space+= sizeof(int);
				}
			else
				break;
		// sprawdzenie zerowego obiektu
		if( p->object_count == 1 )
			if( p->object_offset[0] == STORE_PAGESIZE ) {
				p->object_count--;
				p->free_space+= sizeof(int);
				// efektem wykonania tego powinna byc pusta strona
				// z prawidlowymi wartosciami sterujacymi np. free_space
			}
		
		memset( &(p->object_offset[p->object_count]), 0,
			( (p->object_count <= 0) ? 
			(unsigned)&(p->bytes[STORE_PAGESIZE]) :
			(unsigned)&(p->bytes[p->object_offset[p->object_count-1]]) ) -
			(unsigned)&(p->object_offset[p->object_count]) );
		
		pagemgr->updateFreeMap(pPtr);
		
		pPtr->release(1);
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::deleteObject done: %s\n", object->toString().c_str());
#endif		
		return 0;
	}
	
	int DBStoreManager::modifyObject(TransactionID* tid, ObjectPointer*& object, DataValue* dv)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::modifyObject start...";
#endif		
		deleteObject(tid, object);
		ObjectPointer* newobj;
		createObject(tid, object->getName(), dv, newobj, object->getLogicalID(), object->getIsRoot());
		delete object;
		object = newobj;
#ifdef DEBUG_MODE
		*ec << "Store::Manager::modifyObject done";
#endif
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
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getRoots(ALL) begin..";
#endif
		int rval = getRoots(tid, "", p_roots);

#ifdef DEBUG_MODE
		*ec << "Store::Manager::getRoots(ALL) done";
#endif
		return rval;
	}

	int DBStoreManager::getRoots(TransactionID* tid, string p_name, vector<ObjectPointer*>*& p_roots)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getRoots(BY NAME) begin..";
#endif
		p_roots = new vector<ObjectPointer*>(0);
		vector<int>* rvec;
		rvec = roots->getRoots(p_name.c_str(), tid->getId(), tid->getTimeStamp());
		
		vector<int>::iterator obj_iter;
		for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++)
		{
			ObjectPointer* optr = NULL;
			LogicalID* lid = new DBLogicalID((*obj_iter));
			int rval = getObject(tid, lid, Store::Write, optr);
			if(optr || rval)
				p_roots->push_back(optr);
			else
				return -1;
		}

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getRoots(BY NAME) done: size=%d\n", p_roots->size());
#endif
		return 0;
	}

	int DBStoreManager::getRootsLID(TransactionID* tid, vector<LogicalID*>*& p_roots)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getRootsLID(ALL) begin..";
#endif
		int rval = getRootsLID(tid, "", p_roots);

#ifdef DEBUG_MODE
		*ec << "Store::Manager::getRootsLID(ALL) done";
#endif
		return rval;
	}

	int DBStoreManager::getRootsLID(TransactionID* tid, string p_name, vector<LogicalID*>*& p_roots)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getRootsLID(BY NAME) begin..";
#endif
		p_roots = new vector<LogicalID*>(0);
		vector<int>* rvec;
		rvec = roots->getRoots(p_name.c_str(), tid->getId(), tid->getTimeStamp());
		
		vector<int>::iterator obj_iter;
		for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++)
		{
			LogicalID* lid = new DBLogicalID((*obj_iter));
			p_roots->push_back(lid);
		}

		delete rvec;
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getRootsLID(BY NAME) done: size=%d\n", p_roots->size());
#endif
		return 0;
	}


	int DBStoreManager::addRoot(TransactionID* tid, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::addRoot begin..";
#endif
		int lid = object->getLogicalID()->toInteger();
//		unsigned log_id;
//		int itid = tid==NULL ? -1 : tid->getId();
#ifdef LOGS
		// klon dla logow
//		log->addRoot(itid, object->getLogicalID()->clone(), log_id);
#endif

		object->setIsRoot(true);
		modifyObject(tid, object, object->getValue());

//		*ec << "Store::Manager::modifyObject start...";
//		
//		deleteObject(tid, object);
//		ObjectPointer* newobj;
//		createObject(tid, object->getName(), object->getValue(), newobj, object->getLogicalID(), true);
//		delete object;
//		object = newobj;
//				
//		*ec << "Store::Manager::modifyObject done";

		roots->addRoot(lid, object->getName().c_str(), tid->getId(), tid->getTimeStamp());

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::addRoot done: %s\n", object->toString().c_str());
#endif
		return 0;
	}

	int DBStoreManager::removeRoot(TransactionID* tid, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::removeRoot begin..";
#endif
		int lid = object->getLogicalID()->toInteger();
//		unsigned log_id;
//		int itid = tid==NULL ? -1 : tid->getId();
#ifdef LOGS
		// klon dla logow
//		log->removeRoot(itid, object->getLogicalID()->clone(), log_id);
#endif

		object->setIsRoot(false);
		modifyObject(tid, object, object->getValue());

//		*ec << "Store::Manager::modifyObject start...";
//		
//		deleteObject(tid, object);
//		ObjectPointer* newobj;
//		createObject(tid, object->getName(), object->getValue(), newobj, object->getLogicalID(), false);
//		delete object;
//		object = newobj;
//		
//		*ec << "Store::Manager::modifyObject done";
		
		roots->removeRoot(lid, tid->getId(), tid->getTimeStamp());

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::removeRoot done: %s\n", object->toString().c_str());
#endif
		return 0;
	}

	int DBStoreManager::abortTransaction(TransactionID* tid)
	{
		int err = 0;

		if (!(err = roots->abortTransaction(tid->getId())))
			return err;
		if (!(err = views->abortTransaction(tid->getId())))
			return err;

		return err;
	}

	int DBStoreManager::commitTransaction(TransactionID* tid)
	{
		int err = 0;

		if (!(err = roots->commitTransaction(tid->getId())))
			return err;
		if (!(err = views->commitTransaction(tid->getId())))
			return err;

		return err;
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
