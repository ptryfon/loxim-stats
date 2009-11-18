#include <iostream>
#include <cstdlib>
#include <Store/DBStoreManager.h>
#include <Store/DBDataValue.h>
#include <Store/SystemViews.h>
#include <Indexes/IndexManager.h>
#include <SystemStats/Statistics.h>

using namespace SystemStatsLib;

#define LOGS

namespace Store
{
	StoreManager* StoreManager::theStore = NULL;

	DBStoreManager::DBStoreManager()
	{
		ec = &ErrorConsole::get_instance(EC_STORE);
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
		this->classes = new Classes();
		this->interfaces = new Interfaces();
		this->schemas = new StoreSchemas();
		this->systemviews = new SystemViews();
		this->pagemgr = new PageManager(this);

		this->roots->init(this->buffer, this->log);
		this->views->init(this->buffer, this->log);
		this->classes->init(this->buffer, this->log);
		this->interfaces->init(this->buffer, this->log);
		this->schemas->init(this->buffer, this->log);
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

	Classes* DBStoreManager::getClasses()
	{
		return classes;
	}

	Interfaces* DBStoreManager::getInterfaces()
	{
	    return interfaces;
	}
	
	StoreSchemas* DBStoreManager::getSchemas()
	{
	    return schemas;
	}

	SystemViews* DBStoreManager::getSystemViews()
	{
		return systemviews;
	}


	PageManager* DBStoreManager::getPageManager()
	{
		return pagemgr;
	}

	int DBStoreManager::getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object)
	{
			if(lid->toInteger() & 0xFF000000) {
				debug_printf(*ec, "Store::Manager::getObject from systemViews LID = %u\n", lid->toInteger());
			/* Obiekty widoku systemowego, gdy zapalone są bity 31-24*/
			return systemviews->getObject( tid, lid, mode, object);
			}

		physical_id *p_id = NULL;
		if( (map->getPhysicalID(tid, lid->toInteger(),&p_id)) == 2 ) return 2; //out of range
//		cout << "file: " << p_id->file_id << ", page: " << p_id->page_id << ", off: " << p_id->offset <<endl;

		if (map->equal(p_id, map->RIP))
		{
			debug_print(*ec,  "Store::Manager::getObject failed: Object not found\n(brak ustalonego kodu bledu dla tej operacji, default -> return 2);");
			object = NULL;
			return 0;
		}

		PagePointer *pPtr = buffer->getPagePointer(tid, p_id->file_id, p_id->page_id);

		pPtr->aquire(tid);

		int rval = PageManager::deserialize(tid, pPtr, p_id->offset, object);

		pPtr->release(tid, 0);
                //gtimoszuk
                
                if (pPtr != NULL) {
                    delete pPtr;
                }
                
		if(rval) {
			debug_print(*ec,  "Store::Manager::getObject failed");
			return -1;
		}

		IndexManager::getHandle()->setIndexMarker(lid, object);

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
		Statistics::get_statistics().get_transactions_stats().increment_create_object();

		LogicalID* lid;
		if(p_lid == NULL)
			lid = new DBLogicalID(map->createLogicalID(tid));
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

#ifdef DEBUG_MODE
		sObj.info();
#endif

		int freepage = pagemgr->getFreePage(tid, sObj.size); // strona z wystaraczajaca iloscia miejsca na nowy obiekt
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::createObject freepage = %d\n", freepage);
#endif
		PagePointer* pPtr = buffer->getPagePointer(tid, STORE_FILE_DEFAULT, freepage);

		pPtr->aquire(tid);

		int pidoffset;
		PageManager::insertObject(tid, pPtr, sObj, &pidoffset, log_id);

		pagemgr->updateFreeMap(tid, pPtr);

		pPtr->release(tid, 1);

		physical_id pid;
		pid.page_id = freepage;
		pid.file_id = STORE_FILE_DEFAULT;
		pid.offset = pidoffset;
		map->setPhysicalID(tid, lid->toInteger(), &pid);

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
		Statistics::get_statistics().get_transactions_stats().increment_delete_object();

		unsigned log_id;
		int itid = tid==NULL ? -1 : tid->getId();
#ifdef LOGS
		// klony dla logow
		log->write(itid, object->getLogicalID()->clone(), object->getName(), object->getValue()->clone(), NULL, log_id);
#endif
		physical_id *p_id = NULL;
		if( (map->getPhysicalID(tid, object->getLogicalID()->toInteger(),&p_id)) == 2 ) {
			debug_printf(*ec, "Store::Manager::deleteObject failed: LID=%d out of range\n(brak ustalonego kodu bledu dla tej operacji, default -> return 2;\n", object->getLogicalID()->toInteger());
			return 2; //out of range
		}
#ifdef DEBUG_MODE
		ec->printf("file: %d, page: %d, offset: %d\n", p_id->file_id, p_id->page_id, p_id->offset);
#endif
		if((!p_id->file_id) && (!p_id->page_id) && (!p_id->offset)) return 2;
		if( p_id->offset > STORE_PAGESIZE/4 ) {
			debug_print(*ec,  "Store::Manager::deleteObject failed: invalid offset");
			return 3;
		}
		PagePointer *pPtr = buffer->getPagePointer(tid, p_id->file_id, p_id->page_id);

		pPtr->aquire(tid);

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
		map->setPhysicalID(tid, object->getLogicalID()->toInteger(), map->RIP);

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
					debug_print(*ec,  "Store::Manager::deleteObject corrupted page offsets detected");
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
			(char*)&(p->bytes[STORE_PAGESIZE]) :
			(char*)&(p->bytes[p->object_offset[p->object_count-1]]) ) -
			(char*)&(p->object_offset[p->object_count]) );

		pagemgr->updateFreeMap(tid, pPtr);

		pPtr->release(tid, 1);
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
		Statistics::get_statistics().get_transactions_stats().increment_modify_object();

		string parentRoot;
		if (object->isRoot()) {
			parentRoot = object->getName();
		}
		deleteObject(tid, object);
		ObjectPointer* newobj;
		createObject(tid, object->getName(), dv, newobj, object->getLogicalID(), object->getIsRoot());
		delete object;
		newobj->setParentRoot(parentRoot);
		object = newobj;
#ifdef DEBUG_MODE
		*ec << "Store::Manager::modifyObject done";
#endif
		return 0;
	}

	int DBStoreManager::replaceDV(ObjectPointer* object, DataValue* dv)
	{
		TransactionID t(-1, 99999, (int*)NULL);
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
		rvec = roots->getRoots(tid, p_name.c_str());

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

	int DBStoreManager::getRootsLIDWithBegin(TransactionID* tid, string nameBegin, vector<LogicalID*>*& p_roots) {
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getRootsLIDWithBegin(BY NAME_BEGIN) begin..";
#endif
		p_roots = new vector<LogicalID*>(0);
		vector<int>* rvec;
		rvec = roots->getRootsWithBegin(tid, nameBegin.c_str());

		vector<int>::iterator obj_iter;
		for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++)
		{
			LogicalID* lid = new DBLogicalID((*obj_iter));
			p_roots->push_back(lid);
		}

		delete rvec;
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getRootsLIDWithBegin(BY NAME_BEGIN) done: size=%d\n", p_roots->size());
#endif
		return 0;
	}

	int DBStoreManager::getRootsLID(TransactionID* tid, string p_name, vector<LogicalID*>*& p_roots)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getRootsLID(BY NAME) begin..";
#endif
		p_roots = new vector<LogicalID*>(0);
		vector<int>* rvec;
		rvec = roots->getRoots(tid, p_name.c_str());

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

		roots->addRoot(tid, lid, object->getName().c_str());

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

		roots->removeRoot(tid, lid);

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::removeRoot done: %s\n", object->toString().c_str());
#endif
		return 0;
	}





	int DBStoreManager::getViewsLID(TransactionID* tid, vector<LogicalID*>*& p_views)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getViewsLID(ALL) begin..";
#endif
		int rval = getViewsLID(tid, "", p_views);

#ifdef DEBUG_MODE
		*ec << "Store::Manager::getViewsLID(ALL) done";
#endif
		return rval;
	}

	int DBStoreManager::getViewsLID(TransactionID* tid, string p_name, vector<LogicalID*>*& p_views)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getViewsLID(BY NAME) begin..";
#endif
		p_views = new vector<LogicalID*>(0);
		vector<int>* rvec;
		rvec = views->getViews(tid, p_name.c_str());

		vector<int>::iterator obj_iter;
		for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++)
		{
			LogicalID* lid = new DBLogicalID((*obj_iter));
			p_views->push_back(lid);
		}

		delete rvec;
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getViewsLID(BY NAME) done: size=%d\n", p_views->size());
#endif
		return 0;
	}


	int DBStoreManager::addView(TransactionID* tid, const char* name, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::addView begin..";
#endif
		int lid = object->getLogicalID()->toInteger();

		views->addView(tid, lid, name);

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::addView done: %s\n", name);
#endif
		return 0;
	}

	int DBStoreManager::removeView(TransactionID* tid, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::removeView begin..";
#endif
		int lid = object->getLogicalID()->toInteger();

		views->removeView(tid, lid);

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::removeView done: %s\n", object->toString().c_str());
#endif
		return 0;
	}

	//Classes

int DBStoreManager::getClassesLID(TransactionID* tid, vector<LogicalID*>*& p_classes)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getClassesLID(ALL) begin..";
#endif
		int rval = getClassesLID(tid, "", p_classes);

#ifdef DEBUG_MODE
		*ec << "Store::Manager::getClassesLID(ALL) done";
#endif
		return rval;
	}

	int DBStoreManager::getClassesLID(TransactionID* tid, string p_name, vector<LogicalID*>*& p_classes)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getClassesLID(BY NAME) begin..";
#endif
		p_classes = new vector<LogicalID*>(0);
		vector<int>* rvec;
		rvec = classes->getItems(tid, p_name.c_str());

		vector<int>::iterator obj_iter;
		for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++)
		{
			LogicalID* lid = new DBLogicalID((*obj_iter));
			p_classes->push_back(lid);
		}

		delete rvec;
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getClassesLID(BY NAME) done: size=%d\n", p_classes->size());
#endif
		return 0;
	}

	int DBStoreManager::getClassesLIDByInvariant(TransactionID* tid, string invariantName, vector<LogicalID*>*& p_classes) {
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getClassesLIDByInvariant(BY NAME) begin..";
#endif
		p_classes = new vector<LogicalID*>(0);
		vector<int>* rvec;
		rvec = classes->getClassByInvariant(tid, invariantName.c_str());
		vector<int>::iterator obj_iter;
		for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++)
		{
			LogicalID* lid = new DBLogicalID((*obj_iter));
			p_classes->push_back(lid);
		}

		delete rvec;
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getClassesLIDByInvariant(BY NAME) done: size=%d\n", p_classes->size());
#endif
		return 0;
	}


	int DBStoreManager::addClass(TransactionID* tid, const char* name, const char* invariantName, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::addClass begin..";
#endif
		int lid = object->getLogicalID()->toInteger();

		classes->addClass(tid, lid, name, invariantName);

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::addClass done: %s\n", name);
#endif
		return 0;
	}

	int DBStoreManager::removeClass(TransactionID* tid, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::removeClass begin..";
#endif
		int lid = object->getLogicalID()->toInteger();

		classes->removeItem(tid, lid);

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::removeClass done: %s\n", object->toString().c_str());
#endif
		return 0;
	}


//Interfaces

    int DBStoreManager::getInterfacesLID(TransactionID* tid, vector<LogicalID*>*& p_interfaces)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getInterfacesLID(ALL) begin..";
#endif
		int rval = getInterfacesLID(tid, "", p_interfaces);

#ifdef DEBUG_MODE
		*ec << "Store::Manager::getInterfacesLID(ALL) done";
#endif
		return rval;
	}

	int DBStoreManager::getInterfacesLID(TransactionID* tid, string name, vector<LogicalID*>*& interfacesVec)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getInterfacesLID(BY NAME) begin..";
#endif
		interfacesVec = new vector<LogicalID*>(0);
		vector<int>* rvec;
		rvec = interfaces->getItems(tid, name.c_str());

		debug_print(*ec,  "Store::Manager::getInterfacesLID(BY NAME) past getItems");

		vector<int>::iterator obj_iter;
		for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++)
		{
			LogicalID* lid = new DBLogicalID((*obj_iter));
			interfacesVec->push_back(lid);
		}

		delete rvec;
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getInterfacesLID(BY NAME) done: size=%d\n", interfacesVec->size());
#endif
		return 0;
	}

	int DBStoreManager::addInterface(TransactionID* tid, const string& name, const string& objectName, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::addInterface begin..";
#endif
		int lid = object->getLogicalID()->toInteger();

		debug_printf(*ec, "Store::Manager::addInterface lid = %d\n", lid);

		int err = interfaces->addInterface(tid, lid, name, objectName);

		debug_printf(*ec, "Store::Manager::addInterface after interfaces->addInterface");

		if (err != 0) {
		    debug_print(*ec,  "Store::Manager::addInterface: error in Interfaces::addInterface");
		    return err;
		}

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::addInterface done: %s\n", name);
#endif
		return 0;
	}

	int DBStoreManager::bindInterface(TransactionID* tid, const string& name, const string& bindName)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::bindInterface begin..";
#endif
		int err = interfaces->bindInterface(tid, name, bindName);

		if (err != 0) {
		    debug_print(*ec,  "Store::Manager::bindInterface: error in Interfaces::bindInterface");
		    return err;
		}

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::bindInterface done: %s\n", name);
#endif
		return 0;
	}
	
	int DBStoreManager::getInterfaceBindForObjectName(TransactionID* tid, const string& oName, string& iName, string& bName)
	{
	    return interfaces->getInterfaceBindForObjectName(tid, oName, iName, bName);
	}


	int DBStoreManager::removeInterface(TransactionID* tid, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::removeInterface begin..";
#endif
		int lid = object->getLogicalID()->toInteger();

		interfaces->removeItem(tid, lid);

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::removeInterface done: %s\n", object->toString().c_str());
#endif
		return 0;
	}
	
	
//Schemas
    int DBStoreManager::getSchemasLID(TransactionID* tid, vector<LogicalID*>*& p_schemas)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getSchemasLID(ALL) begin..";
#endif
		int rval = getSchemasLID(tid, "", p_schemas);

#ifdef DEBUG_MODE
		*ec << "Store::Manager::getSchemasLID(ALL) done";
#endif
		return rval;
	}

	int DBStoreManager::getSchemasLID(TransactionID* tid, string name, vector<LogicalID*>*& schemasVec)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::getSchemasLID(BY NAME) begin..";
#endif
		schemasVec = new vector<LogicalID*>(0);
		vector<int>* rvec;
		rvec = schemas->getItems(tid, name.c_str());

		vector<int>::iterator obj_iter;
		for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++)
		{
			LogicalID* lid = new DBLogicalID((*obj_iter));
			schemasVec->push_back(lid);
		}

		delete rvec;
#ifdef DEBUG_MODE
		ec->printf("Store::Manager::getSchemasLID(BY NAME) done: size=%d\n", schemasVec->size());
#endif
		return 0;
	}

	int DBStoreManager::addSchema(TransactionID* tid, string name, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::addSchema begin..";
#endif
		int lid = object->getLogicalID()->toInteger();

		debug_printf(*ec, "Store::Manager::addSchema lid = %d\n", lid);

		int err = schemas->addItem(tid, lid, name.c_str());

		if (err != 0) {
		    debug_print(*ec,  "Store::Manager::addSchema: error in Schemas::addSchema");
		    return err;
		}

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::addSchema done: %s\n", name);
#endif
		return 0;
	}
	
	int DBStoreManager::removeSchema(TransactionID* tid, ObjectPointer*& object)
	{
#ifdef DEBUG_MODE
		*ec << "Store::Manager::removeSchema begin..";
#endif
		int lid = object->getLogicalID()->toInteger();

		schemas->removeItem(tid, lid);

#ifdef DEBUG_MODE
		ec->printf("Store::Manager::removeSchema done: %s\n", object->toString().c_str());
#endif
		return 0;
	}

	
	
//System Views
	int DBStoreManager::getSystemViewsLID(Transaction* tr, vector<LogicalID*>*& p_systemviews)
	{
		#ifdef DEBUG_MODE
			*ec << "Store::Manager::getSystemViewsLID(ALL) begin..";
		#endif
			int rval = getSystemViewsLID(tr, "", p_systemviews);

		#ifdef DEBUG_MODE
			*ec << "Store::Manager::getSystemViewsLID(ALL) done";
		#endif
			return rval;
	}

	int DBStoreManager::getSystemViewsLID(Transaction* tr, string name, vector<LogicalID*>*& p_systemviews)
	{
		#ifdef DEBUG_MODE
				*ec << "Store::Manager::getSystemViewsLID(BY NAME) begin..";
		#endif
				p_systemviews = new vector<LogicalID*>(0);
				vector<int>* rvec;
				rvec = systemviews->getItems(tr, name.c_str());

				vector<int>::iterator obj_iter;
				for(obj_iter=rvec->begin(); obj_iter!=rvec->end(); obj_iter++)
				{
					LogicalID* lid = new DBLogicalID((*obj_iter));
					p_systemviews->push_back(lid);
				}

				delete rvec;
		#ifdef DEBUG_MODE
				ec->printf("Store::Manager::getSystemViewsLID(BY NAME) done: size=%d\n", p_systemviews->size());
		#endif
				return 0;
	}


	int DBStoreManager::abortTransaction(TransactionID* tid)
	{
		int err = 0;

		err = roots->abortTransaction(tid);
		if (err != 0)
			return err;
		err = views->abortTransaction(tid);
		if (err != 0)
			return err;
		err = classes->abortTransaction(tid);
		if (err != 0)
			return err;
		err = interfaces->abortTransaction(tid);
		if (err != 0)
			return err;
		err = schemas->abortTransaction(tid);
		if (err != 0)
			return err;

		return err;
	}

	int DBStoreManager::commitTransaction(TransactionID* tid)
	{
		int err = 0;
		err = roots->commitTransaction(tid);
		if (err != 0)
			return err;
		err = views->commitTransaction(tid);
		if (err != 0)
			return err;
		err = classes->commitTransaction(tid);
		if (err != 0)
			return err;
		err = interfaces->commitTransaction(tid);
		if (err != 0)
			return err;
		err = interfaces->commitTransaction(tid);
		if (err != 0)
			return err;
		err = schemas->commitTransaction(tid);
		if (err != 0)
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

	LogicalID* DBStoreManager::createLID(unsigned int lid)
	{
		return new DBLogicalID(lid);
	}

	int DBStoreManager::logicalIDFromByteArray(unsigned char* buffer, LogicalID*& lid)
	{
		DBLogicalID* dlid;
		int rval = DBLogicalID::deserialize(buffer, dlid);
		lid = dlid;
		return rval;
	}

	int DBStoreManager::dataValueFromByteArray(TransactionID* tid, unsigned char* buffer, DataValue*& value)
	{
		DBDataValue* dvalue;
		int rval = DBDataValue::deserialize(tid, buffer, dvalue);
		value = dvalue;
		return rval;
	}

	DBPhysicalID* DBStoreManager::getPhysicalID(TransactionID* tid, LogicalID* lid)
	{
		physical_id* pid = NULL;
		map->getPhysicalID(tid, lid->toInteger(), &pid);
		if (pid == NULL)
			return NULL;
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
