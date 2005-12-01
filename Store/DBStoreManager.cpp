#include <iostream>
#include "DBStoreManager.h"
#include "DBDataValue.h"

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
		return 0;
	};

	int DBStoreManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object)
	{
		LogicalID* lid = new DBLogicalID(misc->lastlid++);
		
		object = new DBObjectPointer(name, value, lid);
		
		misc->vect.push_back(object);
		
		return 0;
	};

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

		return 0;
	};

	int DBStoreManager::getRoots(TransactionID* tid, string p_name, vector<ObjectPointer*>*& roots)
	{
		roots = new vector<ObjectPointer*>(0);

		for(unsigned int i=0; i<misc->vect.size(); i++){
			if(misc->vect[i]->getName() == p_name)
				roots->push_back(misc->vect[i]);
		}

		return 0;
	};

	int DBStoreManager::addRoot(TransactionID* tid, ObjectPointer* object)
	{
		for(unsigned int i=0; i<misc->roots.size(); i++){
			if(misc->vect[i]->getName() == object->getName())
			  return 1;
		}
		
		misc->roots.push_back(object);
		
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

