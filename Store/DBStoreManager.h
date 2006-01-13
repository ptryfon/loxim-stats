#ifndef __STORE_DBSTOREMANAGER_H__
#define __STORE_DBSTOREMANAGER_H__

namespace Store
{
	class DBStoreManager;
}

#include "Store.h"
#include "Buffer.h"
#include "Map.h"
#include "Roots.h"
#include "DBObjectPointer.h"

using namespace std;
using namespace Config;
using namespace Logs;

namespace Store
{
	class DBStoreManager : public StoreManager
	{
	private:
		SBQLConfig* config;
		LogManager* log;
		Buffer* buffer;
		Map* map;
		Roots* roots;
		unsigned timer;

	public:
		DBStoreManager();
		~DBStoreManager();

		Misc* misc;
		int init(SBQLConfig* config, LogManager* log);
		int start();
		int stop();

		SBQLConfig* getConfig();
		LogManager* getLogManager();
		Buffer* getBuffer();
		Map* getMap();
		Roots* getRoots();

		virtual int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object);
		virtual int createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object);
		virtual int deleteObject(TransactionID* tid, ObjectPointer* object);

		virtual int getRoots(TransactionID* tid, vector<ObjectPointer*>*& roots);
		virtual int getRoots(TransactionID* tid, string name, vector<ObjectPointer*>*& roots);
		virtual int addRoot(TransactionID* tid, ObjectPointer* object);
		virtual int removeRoot(TransactionID* tid, ObjectPointer* object);

		virtual int abortTransaction(TransactionID* tid);
		virtual int commitTransaction(TransactionID* tid);

		virtual DataValue* createIntValue(int value);
		virtual DataValue* createDoubleValue(double value);
		virtual DataValue* createStringValue(string value);
		virtual DataValue* createVectorValue(vector<ObjectPointer*>* value);
		virtual DataValue* createPointerValue(ObjectPointer* value);

		virtual int logicalIDFromByteArray(unsigned char* buffer, int length, LogicalID** lid);
		virtual int dataValueFromByteArray(unsigned char* buffer, int length, DataValue** value);
	};
};

#endif
