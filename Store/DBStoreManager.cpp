#include <iostream>
#include <cstdlib>
#include "DBStoreManager.h"
#include "DBDataValue.h"

namespace Store
{
	StoreManager* StoreManager::theStore = NULL;

	DBStoreManager::DBStoreManager()
	{
		misc = new Misc();
		StoreManager::theStore = this;
	};

	DBStoreManager::~DBStoreManager()
	{
		if (buffer)
		{
			buffer->stop();
			delete buffer;
			buffer = 0;
		}

		if (map)
		{
			delete map;
			map = 0;
		}

		StoreManager::theStore = NULL;
	};

	int DBStoreManager::init(SBQLConfig* config, LogManager* log)
	{
		this->config = config;
		this->log = log;
		this->buffer = new Buffer(this);
		this->map = new Map(this);
		this->roots = new Roots(this);

		return 0;
	};

	int DBStoreManager::start()
	{
		int retval = 0;

		if ((retval = buffer->start()) != 0)
			return retval;

		return 0;
	};

	int DBStoreManager::stop()
	{
		int retval = 0;

		if ((retval = buffer->stop()) != 0)
			return retval;

		return 0;
	};

	SBQLConfig* DBStoreManager::getConfig()
	{
		return config;
	};

	LogManager* DBStoreManager::getLogManager()
	{
		return log;
	};

	Buffer* DBStoreManager::getBuffer()
	{
		return buffer;
	}

	Map* DBStoreManager::getMap()
	{
		return map;
	};
	
	Roots* DBStoreManager::getRoots()
	{
		return roots;
	};

	int DBStoreManager::getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object)
	{
		object = NULL;
		LogicalID* l;
		for(unsigned int i=0; i<misc->vect.size(); i++)
		{
			l = (misc->vect[i])->getLogicalID();
			if( (*l) == (*lid) ) {
				object = misc->vect[i];
				break;
			}
		}
		cout << "Store::Manager::getObject done: " + object->toString() + "\n";
		return 0;
	};

//	int DBStoreManager::getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object)
//	{
//		cout << "Store::Manager::getObject started..\n";
//		
//		physical_id *p_id;
//		map->getPhysicalID(lid->toInteger(), &p_id);
//		PagePointer *pPtr = buffer->getPagePointer(p_id->file_id, p_id->page_id);
//		
//		pPtr->aquire();
//		
//		page_data = reinterpret_cast<page_data*>(pPtr->getPage());
//		int rval = PageManager::deserialize(pPtr, p_id->offset, object);
//		
//		pPtr->release();
//		
//		if(rval) {
//			cout << "Store::Manager::getObject failed\n";
//			return -1;
//		}
//		cout << "Store::Manager::getObject done: " + object->toString();
//		return 0;	
//	};
	
	int DBStoreManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object)
	{
		cout << "Store::Manager::createObject start..\n";
		
		LogicalID* lid = new DBLogicalID(misc->lastlid++);
		
		object = new DBObjectPointer(name, value, lid);
		
		misc->vect.push_back(object);
		
		cout << "Store::Manager::createObject done: " + object->toString() + "\n";
		return 0;
	};

//	int DBStoreManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object)
//	{
//		cout << "Store::Manager::createObject start..\n";
//		
//		if( (value->getType()!=Store::Integer) &&
//			(value->getType()!=Store::Double) &&
//			(value->getType()!=Store::String) )
//		{
//			cout << "Store::createObject: Illegal or not implemented value type\n";
//			return -1;
//		}
//
//		//mapa sie wywala
//		LogicalID* lid = new DBLogicalID(/*map->createLogicalID()*/misc->lastlid++);
//		
//		object = new DBObjectPointer(name, value, lid);
//
//		Serialized sObj = object->serialize();
//
//		int freepage = PageManager::getFreePage(); // strona z wystaraczajaca iloscia miejsca na nowy obiekt
//
//		PagePointer *pPtr = buffer->getPagePointer(STORE_FILE_DEFAULT, freegame);
//
//		pPtr->aquire();
//
//		// BEGIN SETHEADERS
//		// PageManager::writeHeader(pPtr); // sprawdzic czy stroa jest zainicjalizowana, jesli nie do ustawic reszte pol
//		// END SETHEADERS
//
//		// COPY OBJECT to PAGE
//		PageManager::insertObject(pPtr, sObj);
//
//		PageManager::addToFreeMap(pPtr);
//		
//		pPtr->release();
//		
//		cout << "Store::Manager::createObject done: " + object->toString() + "\n";
//		return 0;
//	};

	int DBStoreManager::deleteObject(TransactionID* tid, ObjectPointer* object)
	{
		return 0;
	};

	int DBStoreManager::getRoots(TransactionID* tid, vector<ObjectPointer*>*& roots)
	{
		//roots = new vector<ObjectPointer*>(0);

		//(roots)->push_back(new DBObjectPointer());
		//(roots)->push_back(new DBObjectPointer());
		//(roots)->push_back(new DBObjectPointer());

//		roots = &(misc->roots);

		roots = new vector<ObjectPointer*>(0);

		for(unsigned int i=0; i<misc->vect.size(); i++){
			roots->push_back(misc->vect[i]);
		}

		cout << "Store::Manager::getRoots(ALL) done\n";
		return 0;
	};

	int DBStoreManager::getRoots(TransactionID* tid, string p_name, vector<ObjectPointer*>*& roots)
	{
		roots = new vector<ObjectPointer*>(0);

		for(unsigned int i=0; i<misc->vect.size(); i++){
			if(misc->vect[i]->getName() == p_name)
				roots->push_back(misc->vect[i]);
		}

		cout << "Store::Manager::getRoots(BY NAME) done\n";
		return 0;
	};

	int DBStoreManager::addRoot(TransactionID* tid, ObjectPointer* object)
	{
		for(unsigned int i=0; i<misc->roots.size(); i++){
			if(misc->vect[i]->getName() == object->getName())
			  return 0;
		}
		
		misc->roots.push_back(object);
		
		cout << "Store::Manager::addRoot done: " + object->toString() + "\n";
		return 0;
	};

	int DBStoreManager::removeRoot(TransactionID* tid, ObjectPointer* object)
	{
		return 0;
	};

	int DBStoreManager::abortTransaction(TransactionID* tid)
	{
		return 0;
	};

	int DBStoreManager::commitTransaction(TransactionID* tid)
	{
		return 0;
	};

	DataValue* DBStoreManager::createIntValue(int value)
	{
		return new DBDataValue(value);
	};

	DataValue* DBStoreManager::createDoubleValue(double value)
	{
		return new DBDataValue(value);
	};

	DataValue* DBStoreManager::createStringValue(string value)
	{
		return new DBDataValue(value);
	};

	DataValue* DBStoreManager::createPointerValue(LogicalID* value)
	{
//		return new DBDataValue(value);
		return 0;
	};

	DataValue* DBStoreManager::createVectorValue(vector<LogicalID*>* value)
	{
//		return new DBDataValue(value);
		return 0;
	};

	ObjectPointer* DBStoreManager::createObjectPointer(LogicalID* lid)
	{
		return new DBObjectPointer(lid);
	};

	ObjectPointer* DBStoreManager::createObjectPointer(LogicalID* lid, string name, DataValue* dv)
	{
		return new DBObjectPointer(name, dv, lid);
	};

	int DBStoreManager::logicalIDFromByteArray(unsigned char* buffer, int length, LogicalID** lid)
	{
		return 0;
	};

	int DBStoreManager::dataValueFromByteArray(unsigned char* buffer, int length, DataValue** value)
	{
		return 0;
	};

	DBPhysicalID* DBStoreManager::getPhysicalID(LogicalID* lid)
	{
		physical_id* pid;
		map->getPhysicalID(lid->toInteger(), &pid);
		return new DBPhysicalID(*pid);
	};
	

//	int DBStoreManager::deleteObject(TransactionID* tid, ObjectPointer* object)
//	{
//		cout << "Store::Manager::deleteObject start..\n";
//		
//		//zapisane do Log
//		unsigned *id;
//		Log::write(tid, object->getLogicalID(), object->getValue(), NULL, &id);
//
//
//		physical_id *p_id;
//		map->getPhysicalID(object->getLogicalID()->toInteger(), &p_id);
//		PagePointer *pPtr = buffer->getPagePointer(p_id->file_id, p_id->page_id);
//		pPtr->aquire();
//		page_data *p = reinterpret_cast<page_data*>(pPtr->getPage());
//		p->header->timestamp= id;		
//
//		int pos_table = p_id->offset;
//		int end_of_object;
//	
//		if ( pos_table == 0 )
//			end_of_object = STORE_PAGESIZE; // koniec strony
//		else	//poczatek poprz obiektu
//			end_of_object = p->object_offset[pos_table-1];
//		
//		// rozmiar usuwanego obiektu
//		int object_size = end_of_object - p->object_offset[pos_table];
//		
//		char* page = pPtr->getPage();
//		
//		// przesuniecie obiektow na stronie
//		int i;
//		for(i = pos_table+1; i <= p->object_count-1; i++)
//		{
//		    char pom[STORE_PAGESIZE];
// 		    
//		    //rozmiar przesuwanego obiektu
//		    int size = p->object_offset[i-1] - p->object_offset[i];
//		    // poczatek przesuwanego obiektu
//		    int start = p->object_offset[i];
//		    
//		    memmove(pom, page + start  , size);
//		    memmove(page + start + object_size, pom, size);
//		    
//		};
//		
//		// uaktualnienie tablicy offsetow
//		// oraz dodanie do starego offsetu rozmiaru usuwanego obiektu
//		for(i = pos_table+1; i <= p->object_count-1; i++)
//		    p->object_offset[i] = p->obcject_offset[i] + object_size;    
//
//		// nagrobek 
//		p->object_offset[i] = -1;		
//
//		// poinformowanie mapy o usunieciu obiektu
//		memset(p_id, 0xFF, sizeof(p_id));
//		map->setPhysicalID(lid, p_id);
//
//		// uaktualnienie info na stronie
//		p->object_count--;
//		p->free_space = p->free_space + object_size;
//		
//		pPtr->release();
//
//		cout << "Store::Manager::deleteObject done: " + object->toString() + "\n";
//		
//		return 0;
//	};
}
