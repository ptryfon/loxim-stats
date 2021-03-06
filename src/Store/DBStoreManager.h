#ifndef __STORE_DBSTOREMANAGER_H__
#define __STORE_DBSTOREMANAGER_H__

namespace Store
{
	class DBStoreManager;
}

#include <Store/Store.h>
#include <Store/Buffer.h>
#include <Store/Map.h>
#include <Store/NamedRoots.h>
#include <Store/Views.h>
#include <Store/DBObjectPointer.h>
#include <Store/PageManager.h>
#include <Store/Misc.h>
#include <Errors/ErrorConsole.h>
#include <Store/NamedItems.h>
#include <Store/SystemViews.h>

using namespace std;
using namespace Config;
using namespace Logs;
using namespace TManager;

namespace Store
{
	class DBStoreManager : public StoreManager
	{
	friend class DBObjectPointer;
	private:
		SBQLConfig* config;
		LogManager* log;
		TransactionManager* tm;
		Buffer* buffer;
		Map* map;
		NamedRoots* roots;
		Views* views;
		Classes* classes;
		Interfaces *interfaces;
		StoreSchemas *schemas;
		SystemViews* systemviews;
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
		virtual NamedRoots* getRoots();
		Views* getViews();
		Classes* getClasses();
		Interfaces* getInterfaces();
		StoreSchemas* getSchemas();
		SystemViews* getSystemViews();
		PageManager* getPageManager();

		virtual int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object);
		virtual int createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object, LogicalID* lid = NULL);
		virtual int createObject(TransactionID* tid, string name, DataValue* value, ObjectPointer*& object, LogicalID* lid, bool isRoot);
		virtual int deleteObject(TransactionID* tid, ObjectPointer* object);
		virtual int modifyObject(TransactionID* tid, ObjectPointer*& object, DataValue* value);

		virtual int replaceDV(ObjectPointer* obj, DataValue* dv);

		virtual int getRoots(TransactionID* tid, vector<ObjectPointer*>*& roots);
		virtual int getRoots(TransactionID* tid, string name, vector<ObjectPointer*>*& roots);
		virtual int getRootsLID(TransactionID* tid, vector<LogicalID*>*& roots);
		virtual int getRootsLID(TransactionID* tid, string name, vector<LogicalID*>*& roots);
		virtual int getRootsLIDWithBegin(TransactionID* tid, string nameBegin, vector<LogicalID*>*& roots);
		virtual int addRoot(TransactionID* tid, ObjectPointer*& object);
		virtual int removeRoot(TransactionID* tid, ObjectPointer*& object);

		virtual int getViewsLID(TransactionID* tid, vector<LogicalID*>*& roots);
		virtual int getViewsLID(TransactionID* tid, string name, vector<LogicalID*>*& roots);
		virtual int addView(TransactionID* tid, const char* name, ObjectPointer*& object);
		virtual int removeView(TransactionID* tid, ObjectPointer*& object);

		virtual int getClassesLID(TransactionID* tid, vector<LogicalID*>*& roots);
		virtual int getClassesLID(TransactionID* tid, string name, vector<LogicalID*>*& roots);
		virtual int getClassesLIDByInvariant(TransactionID* tid, string invariantName, vector<LogicalID*>*& roots);
		virtual int addClass(TransactionID* tid, const char* name, const char* invariantName, ObjectPointer*& object);
		virtual int removeClass(TransactionID* tid, ObjectPointer*& object);

		virtual int getInterfacesLID(TransactionID* tid, vector<LogicalID*>*& roots);
		virtual int getInterfacesLID(TransactionID* tid, string name, vector<LogicalID*>*& roots);
		virtual int addInterface(TransactionID* tid, const string& name, const string& objectName, ObjectPointer*& object);
		virtual int bindInterface(TransactionID* tid, const string& name, const string& bindName);
		virtual int getInterfaceBindForObjectName(TransactionID* tid, const string& oName, string& iName, string& bName); 
		virtual int removeInterface(TransactionID* tid, ObjectPointer*& object);

		virtual int getSchemasLID(TransactionID* tid, vector<LogicalID*>*& s);
		virtual int getSchemasLID(TransactionID* tid, string name, vector<LogicalID*> *&s);
		virtual int addSchema(TransactionID* tid, string name, ObjectPointer *&object);
		virtual int removeSchema(TransactionID* tid, ObjectPointer *&object);

		virtual int getSystemViewsLID(Transaction* tr, vector<LogicalID*>*& p_systemviews);
		virtual int getSystemViewsLID(Transaction* tr, string name, vector<LogicalID*>*& p_systemviews);

		virtual int abortTransaction(TransactionID* tid);
		virtual int commitTransaction(TransactionID* tid);

		virtual DataValue* createIntValue(int value);
		virtual DataValue* createDoubleValue(double value);
		virtual DataValue* createStringValue(string value);
		virtual DataValue* createPointerValue(LogicalID* value);
		virtual DataValue* createVectorValue(vector<LogicalID*>* value);
		virtual LogicalID* createLID(unsigned int);

		virtual ObjectPointer* createObjectPointer(LogicalID* lid);
		virtual ObjectPointer* createObjectPointer(LogicalID* lid, string name, DataValue* dv);

		virtual int logicalIDFromByteArray(unsigned char* buffer, LogicalID*& lid);
		virtual int dataValueFromByteArray(TransactionID* id, unsigned char* buffer, DataValue*& value);

		virtual DBPhysicalID* getPhysicalID(TransactionID* tid, LogicalID* lid);
		virtual int checkpoint(unsigned int& cid);
		virtual int endCheckpoint(unsigned int& cid);
	};
};

#endif
