#include "DBStoreManager.h"

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
		LogicalID* l;
		for(int i=0; i<misc->vect.size(); i++)
		{
			l = (misc->vect[i])->getLogicalID();
		}
		object = new DBObjectPointer();

		return 0;
	};

	int DBStoreManager::createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object)
	{
		
		object = new DBObjectPointer(name, value);
		
		misc->vect.push_back(object);
		
		return 0;
	};

	int DBStoreManager::deleteObject(TransactionID* tid, ObjectPointer* object)
	{
		return 0;
	};

	int DBStoreManager::getRoots(TransactionID* tid, vector<ObjectPointer*>*& roots)
	{
		roots = new vector<ObjectPointer*>(0);

		(roots)->push_back(new DBObjectPointer());
		(roots)->push_back(new DBObjectPointer());
		(roots)->push_back(new DBObjectPointer());

		return 0;
	};

	int DBStoreManager::getRoots(TransactionID* tid, string name, vector<ObjectPointer*>*& roots)
	{
		roots = new vector<ObjectPointer*>(0);

		(roots)->push_back(new DBObjectPointer());

		return 0;
	};

	int DBStoreManager::addRoot(TransactionID* tid, ObjectPointer* object)
	{
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
		return 0;
	};

	DataValue* DBStoreManager::createDoubleValue(double value)
	{
		return 0;
	};

	DataValue* DBStoreManager::createStringValue(string value)
	{
		return 0;
	};

	DataValue* DBStoreManager::createVectorValue(vector<ObjectPointer*>* value)
	{
		return 0;
	};

	DataValue* DBStoreManager::createPointerValue(ObjectPointer* value)
	{
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

