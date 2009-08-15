#ifndef _TRANSACTION_
#define _TRANSACTION_

/**
 *	@author Julian Krzemi�ski (julian.krzeminski@students.mimuw.edu.pl)
 *	@author Dominik Klimczak (dominik.klimczak@students.mimuw.edu.pl)
 */
namespace TManager
{
	class TransactionID;
};

#include <cstdio>
#include <list>
#include <iterator>
#include <vector>
#include <map>

#include <Store/Store.h>
#include <Lock/Lock.h>
#include <Log/Logs.h>
#include <Errors/ErrorConsole.h>
#include <Config/SBQLConfig.h>
#include <QueryParser/ClassNames.h>
#include <TypeCheck/ClassNames.h>
#include <TypeCheck/DMLControl.h>
#include <Util/Concurrency.h>


using namespace Store;
using namespace LockMgr;
using namespace Logs;
using namespace Errors;
using namespace TypeCheck;

namespace Indexes {
	class IndexManager;
	class Tester;
}

namespace TManager
{

  class TransactionManager;

	/**
	 *	TransactionID - unique object for transaction
	 */
	class TransactionID
	{
	      private:
			friend class Transaction;
			friend class TransactionManager;
			TransactionID* clone();
		        int id;
			int priority;
			int sessionId;
			unsigned timeStamp;

	      public:
			int getId() const;
			int getSessionId() const;
			int getPriority() const;
			unsigned getTimeStamp() const;
			void setTimeStamp(unsigned t);
		        TransactionID(int sessionId, int n, int* nothing);
		        TransactionID(int sessionId, int n, int priority);
	};

	/**
	 *   Transaction - handles Executor's calls
	 */
	class Transaction
	{
	      private:
			friend class TransactionManager;
			TransactionID *tid;
			StoreManager* sm;
			LogManager* logm;
			TransactionManager* tm;
			LockManager* lm;
			ErrorConsole &err;
			Util::RWSemaphore* sem;    /* for operation on Store Manager */
			DMLControl *dmlStructs;
	      public:
			Transaction(TransactionID* tId, Util::RWSemaphore* sem);
		    	~Transaction();
				DMLControl *getDmlStct();
				void reloadDmlStct();
	     	 	/* Executor calls: */
		    	int getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p, bool allowNullObject);
			int modifyObject(ObjectPointer* &op, DataValue* dv);
		    	int createObject(string name, DataValue* value, ObjectPointer* &p);
		    	int deleteObject(ObjectPointer* object);

		    	int getRoots(vector<ObjectPointer*>* &p);
		    	int getRoots(string name, vector<ObjectPointer*>* &p);
		    	int getRootsLID(vector<LogicalID*>* &p);
		    	int getRootsLID(string name, vector<LogicalID*>* &p);
		    	int getRootsLIDWithBegin(string nameBegin, vector<LogicalID*>* &p);
		    	int addRoot(ObjectPointer* &p);
		    	int removeRoot(ObjectPointer* &p);

		    	int getViewsLID(vector<LogicalID*>* &p);
		    	int getViewsLID(string name, vector<LogicalID*>* &p);
		    	int addView(const char* name, ObjectPointer* &p);
		    	int removeView(ObjectPointer* &p);

		    	int getClassesLID(vector<LogicalID*>* &p);
		    	int getClassesLID(string name, vector<LogicalID*>* &p);
		    	int getClassesLIDByInvariant(string invariantName, vector<LogicalID*>* &p);
		    	int addClass(const char* name, const char* invariantName, ObjectPointer* &p);
		    	int removeClass(ObjectPointer* &p);

			int getInterfacesLID(vector<LogicalID*>* &p);
		    	int getInterfacesLID(string name, vector<LogicalID*>* &p);

			int getSystemViewsLID(vector<LogicalID*>* &p);
		    	int getSystemViewsLID(string name, vector<LogicalID*>* &p);

		    	int addInterface(const string& name, const string& objectName, ObjectPointer* &p);
		    	int bindInterface(const string& name, const string& bindName);
		    	int getInterfaceBindForObjectName(const string& objectName, string& interfaceName, string& bindName); 
		    	int removeInterface(ObjectPointer* &p);

		    	TransactionID* getId();

				int getSchemasLID(vector<LogicalID*> *&p);
				int getSchemasLID(string name, vector<LogicalID*> *&p);
				int addSchema(const string& name, ObjectPointer *&p);
		    	int removeSchema(ObjectPointer *&p);

			/* Data creation: */
		    	int createIntValue(int value, DataValue* &dataVal);
		    	int createDoubleValue(double value, DataValue* &dataVal);
		    	int createStringValue(string value, DataValue* &dataVal);
		    	int createVectorValue(vector<LogicalID*>* value, DataValue* &dataVal);
		    	int createPointerValue(LogicalID* value, DataValue* &dataVal) ;

			/* Final execution of transacion */
		    	int abort();
		    	int commit();

		private:
			int init(StoreManager*, LogManager*);
	};

	/**
	 *	TransactionManager - factory of Transactions, handles rollback operation
	 *      Sigleton Design Pattern
	 */
	class TransactionManager
	{
		private:
			Util::Mutex* mutex;
			Util::RWSemaphore* sem;
			int transactionId;    /* counter of TransactionID objects */
			static TransactionManager *tranMgr;
			StoreManager* storeMgr;
			LogManager* logMgr;

			int minimalTransactionId;
			bool semaphoresTimeout;
			int readerTimeout;
			int writerTimeout;
			int boostAfterDeadlock;

			TransactionManager();
			int loadConfig();
 			void addTransaction(TransactionID*);
	      		list<TransactionID*>* transactions;
	      		int remove_from_list(TransactionID*);
	      		ErrorConsole err;

	   	public:
	      		~TransactionManager();
	      		static TransactionManager* getHandle();

	   		/* called at server startup: */
	      		static int init(StoreManager*, LogManager*);

	   		/* executor calls: */
	     		int createTransaction(int sessionId, Transaction* &tr);
	     		int createTransaction(int sessionId, Transaction* &tr, int id);
	      		int commit(Transaction*);
	      		int abort(Transaction*);

	   		/* log calls: */
	      		list<TransactionID*>* getTransactions();
	      		vector<int>* getTransactionsIds();

			int getReaderTimeout();
			int getWriterTimeout();

		friend class Indexes::IndexManager;
		friend class Indexes::Tester;
	};
};

#endif
