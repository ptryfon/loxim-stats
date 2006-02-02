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
#include "PageManager.h"
#include "Misc.h"
#include "Errors/ErrorConsole.h"

using namespace std;
using namespace Config;
using namespace Logs;
using namespace TManager;

namespace Store
{
	class DBStoreManager : public StoreManager
	{
	private:
		SBQLConfig* config;
		LogManager* log;
		TransactionManager* tm;
		Buffer* buffer;
		Map* map;
		Roots* roots;
		PageManager* pagemgr;
		unsigned timer;
		ErrorConsole *ec;

	public:
		DBStoreManager();
		~DBStoreManager();

		Misc* misc;
		int init(LogManager* log);
		int setTManager(TransactionManager* tm);
		int start();
		int stop();

		SBQLConfig* getConfig();
		LogManager* getLogManager();
		TransactionManager* getTManager();
		Buffer* getBuffer();
		Map* getMap();
		Roots* getRoots();
		PageManager* getPageManager();

		virtual int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object);
		virtual int createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object, LogicalID* lid=NULL);
		virtual int deleteObject(TransactionID* tid, ObjectPointer* object);
		virtual int modifyObject(TransactionID* tid, ObjectPointer*& object, DataValue* value);

		virtual int replaceDV(ObjectPointer* obj, DataValue* dv);

		virtual int getRoots(TransactionID* tid, vector<ObjectPointer*>*& roots);
		virtual int getRoots(TransactionID* tid, string name, vector<ObjectPointer*>*& roots);
		virtual int addRoot(TransactionID* tid, ObjectPointer* object);
		virtual int removeRoot(TransactionID* tid, ObjectPointer* object);

		virtual int abortTransaction(TransactionID* tid);
		virtual int commitTransaction(TransactionID* tid);

		virtual DataValue* createIntValue(int value);
		virtual DataValue* createDoubleValue(double value);
		virtual DataValue* createStringValue(string value);
		virtual DataValue* createPointerValue(LogicalID* value);
		virtual DataValue* createVectorValue(vector<LogicalID*>* value);
		
		virtual ObjectPointer* createObjectPointer(LogicalID* lid);
		virtual ObjectPointer* createObjectPointer(LogicalID* lid, string name, DataValue* dv);

		virtual int logicalIDFromByteArray(unsigned char* buffer, LogicalID*& lid);
		virtual int dataValueFromByteArray(unsigned char* buffer, DataValue*& value);
		
		virtual DBPhysicalID* getPhysicalID(LogicalID* lid);
		virtual int checkpoint(unsigned int& cid);
		virtual int endCheckpoint(unsigned int& cid);
	};
};

#endif
