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

#include<stdio.h>
#include <list>
#include <iterator>
#include <vector>

#include "../Store/Store.h"
#include "../Lock/Lock.h"
#include "../Log/Logs.h"
#include "../Errors/ErrorConsole.h"
#include "../Config/SBQLConfig.h"
#include "SemHeaders.h"


using namespace Store;
using namespace LockMgr;
using namespace Logs;
using namespace Errors;
using namespace SemaphoreLib;

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
			unsigned timeStamp;

	      public:
			int getId() const;
			int getPriority() const;
			unsigned getTimeStamp() const;
			void setTimeStamp(unsigned t);
		        TransactionID(int n);
		        TransactionID(int n, int priority);
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
			ErrorConsole err;
			Semaphore* sem;    /* for operation on Store Manager */
	
	      public:
			Transaction(TransactionID* tId, Semaphore* sem);
		    	~Transaction();
	    	
	     	 	/* Executor calls: */
		    	int getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p, bool allowNullObject);
			int modifyObject(ObjectPointer* &op, DataValue* dv);
		    	int createObject(string name, DataValue* value, ObjectPointer* &p);
		    	int deleteObject(ObjectPointer* object);
		    
		    	int getRoots(vector<ObjectPointer*>* &p);
		    	int getRoots(string name, vector<ObjectPointer*>* &p);
		    	int getRootsLID(vector<LogicalID*>* &p);
		    	int getRootsLID(string name, vector<LogicalID*>* &p);
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
		    	//int addInterface(const char* name, const char* invariantName, ObjectPointer* &p);
		    	//int removeInterface(ObjectPointer* &p);
	
		    	TransactionID* getId();

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
			Mutex* mutex;
			Semaphore* sem;
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
	      		int init(StoreManager*, LogManager*);
	      
	   		/* executor calls: */
	     		int createTransaction(Transaction* &tr);
	     		int createTransaction(Transaction* &tr, int id);
	      		int commit(Transaction*);
	      		int abort(Transaction*);
	      
	   		/* log calls: */
	      		list<TransactionID*>* getTransactions();
	      		vector<int>* getTransactionsIds();
			
			int getReaderTimeout();
			int getWriterTimeout();
	};
};

#endif
