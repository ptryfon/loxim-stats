#include <iostream>
#include "DBStoreManager.h"
#include "DBDataValue.h"
#include <cstdlib>

namespace Store
{
	DBStoreManager::DBStoreManager()
	{
		misc = new Misc();
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

	int DBStoreManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object)
	{
		cout << "Store::Manager::createObject start..\n";
		
		LogicalID* lid = new DBLogicalID(misc->lastlid++);
		
		object = new DBObjectPointer(name, value, lid);
		
		misc->vect.push_back(object);
		
		cout << "Store::Manager::createObject done: " + object->toString() + "\n";
		return 0;
	};


//	int DBStoreManager::createObjectEx(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object)
//	{
//		cout << "Store::Manager::createObject start..\n";
//		
//		//mapa sie wywala
//		LogicalID* lid = new DBLogicalID(/*map->createLogicalID()*/misc->lastlid++);
//		
//		int size = lid->binarySize() + (2*sizeof unsigned int)//random, namesize
//			+ name.length() + sizeof DataType + value->fullBinarySize();
//		
//		rval = map->locateNewSpace(size);
//		map->setPhisicalID(lid, rval);
//		PagePointer pPtr = buffer->getpagePointer(rval);
/*		
		// BINARIZE OBJECT
		
		//pageOperator->binarize(..);
		
		unsigned char* binaryObject = new unsigned char[size];
		int writePosition = 0;
		int asize = 0;
		unsigned char* adata = NULL;
		unsigned int tmp = 0;
		
		//LogicalID
		lid->toByteArray(&adata, &asize);
		for(int i=0; i<asize; i++)
			binaryObject[writePosition++] = adata[i];
		if(adata) delete[] adata;
		adata = NULL;
		
		//random
		tmp = (rand()%0x100) << 24 + (rand()%0x100) << 16 +
			(rand()%0x100) << 8 + (rand()%0x100);
		adata = static_cast<unsigned char*>(&tmp);
		for(int i=0; i<sizeof unsigned int; i++)
			binaryObject[writePosition++] = adata[i];
		adata = NULL;
		
		//namesize
		tmp = static_cast<unsigned int>(name.length());
		adata = static_cast<unsigned char*>(&tmp);
		for(int i=0; i<sizeof unsigned int; i++)
			binaryObject[writePosition++] = adata[i];
		adata = NULL;

		//name
		asize = name.length();
		for(int i=0; i<asize; i++)
			binaryObject[writePosition++] =
				reinterpret_cast<unsigned char>(name[i]);
		
		//value
		value->toFullByteArray(&adata, &asize);
		for(int i=0; i<asize; i++)
			binaryObject[writePosition++] = adata[i];
		if(adata) delete[] adata;
		adata = NULL;
				
		// END BINARIZE

		pPtr->acquire();

		// BEGIN SETHEADERS
		
		//pageOperator->setHeaders(...);
		
		// END SETHEADERS

		// COPY OBJECT to PAGE
		copy(binaryObject onto pPtr->page);
		
		pPtr->release();
		
		object = new DBObjectPointer(name, value, lid);
		
		//misc->vect.push_back(object);
		
		cout << "Store::Manager::createObject done: " + object->toString() + "\n";
		return 0;
	};
*/

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

		roots = &(misc->roots);

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

	DataValue* DBStoreManager::createPointerValue(ObjectPointer* value)
	{
//		return new DBDataValue(value);
		cout << "Store::Manager::createPointer not implemented" << endl;
		return 0;
	};

	DataValue* DBStoreManager::createVectorValue(vector<ObjectPointer*>* value)
	{
//		return new DBDataValue(value);
		cout << "Store::Manager::createVector not implemented" << endl;
		return 0;
	};

	int DBStoreManager::logicalIDFromByteArray(unsigned char* buffer, int length, LogicalID** lid)
	{
		return 0;
	};

	int DBStoreManager::dataValueFromByteArray(unsigned char* buffer, int length, DataValue** value)
	{
		return 0;
	};
}

