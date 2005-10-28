#include "DBStoreManager.h"
#include "DBObjectPointer.h"

namespace Store
{
	void init(SBQLConfig* config, LogManager* log)
	{
	};

	ObjectPointer* DBStoreManager::getObject(TransactionID* tid, LogicalID* lid, ObjectPointer::AccessMode mode)
	{
		return new DBObjectPointer();
	};

	ObjectPointer* DBStoreManager::createObject(TransactionID* tid, string name, DataValue* value)
	{
		return 0;
	};

	void DBStoreManager::deleteObject(TransactionID* tid, ObjectPointer* object)
	{
	};

	vector<ObjectPointer*>* DBStoreManager::getRoots(TransactionID* tid)
	{
		vector<ObjectPointer*>* v = new vector<ObjectPointer*>(0);

		v->push_back(new DBObjectPointer());
		v->push_back(new DBObjectPointer());
		v->push_back(new DBObjectPointer());

		return v;
	};

	vector<ObjectPointer*>* DBStoreManager::getRoots(TransactionID* tid, string name)
	{
		vector<ObjectPointer*>* v = new vector<ObjectPointer*>(0);

		v->push_back(new DBObjectPointer());

		return v;
	};

	void DBStoreManager::addRoot(TransactionID* tid, ObjectPointer* object)
	{
	};

	void DBStoreManager::removeRoot(TransactionID* tid, ObjectPointer* object)
	{
	};

	void DBStoreManager::abortTransaction(TransactionID* tid)
	{
	};

	void DBStoreManager::commitTransaction(TransactionID* tid)
	{
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

	LogicalID* DBStoreManager::logicalIDFromByteArray(unsigned char* lid, int length)
	{
		return 0;
	};

	DataValue* DBStoreManager::dataValueFromByteArray(unsigned char* value, int length)
	{
		return 0;
	};
}

