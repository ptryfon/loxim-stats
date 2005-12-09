#ifndef _TRANSACTION_
#define _TRANSACTION_

namespace TManager
{
	class TransactionID;
};

#include<stdio.h>
#include <list>
#include <iterator>
#include "../Store/Store.h"
#include "../Lock/Lock.h"
#include "../Log/Logs.h"
#include "../Errors/ErrorConsole.h"
using namespace Store;
using namespace LockMgr;
using namespace Logs;
using namespace Errors;

namespace TManager
{

	/**
	 *	TransactionID - unique object for transaction
	 */
	class TransactionID 
	{      
	      private:
		friend class Transaction;
	        int id;	

	      public:
		int getId() const;	      
	        TransactionID(int n);
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
		int getId();
		ErrorConsole err;
	      public:
		    Transaction();
		    Transaction(TransactionID* tId);
	      /* Executor calls: */
		    int getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p);
		    int createObject(string name, DataValue* value, ObjectPointer* &p);
		    int deleteObject(ObjectPointer* object);
		    
		    int getRoots(vector<ObjectPointer*>* &p);
		    int getRoots(string name, vector<ObjectPointer*>* &p);
		    int addRoot(ObjectPointer* &p);
		    int removeRoot(ObjectPointer* &p);

		// Data creation
		    int createIntValue(int value, DataValue* &dataVal);
		    int createDoubleValue(double value, DataValue* &dataVal);
		    int createStringValue(string value, DataValue* &dataVal);
		    int createVectorValue(vector<ObjectPointer*>* value, DataValue* &dataVal);
		    int createPointerValue(ObjectPointer* value, DataValue* &dataVal) ;

		    int abort();
		    int commit();

	      private:
	    	    int init(StoreManager*, LogManager*);
//		    void setSM(StoreManager*);
	}; 

	/**
	 *	TransactionManager - factory of Transactions
	 *      Sigleton Design Pattern
	 */
	class TransactionManager
	{ 
	   private:
	      int mutex;	    /* mutual exclusion */
	      int transactionId;    /* counter of TransactionID objects */	
	      static TransactionManager *tranMgr;   	   
	      StoreManager* storeMgr;
	      LogManager* logMgr;

	      TransactionManager();
    	      void addTransaction(int);
	      list<int>* transactions;
	      int remove_from_list(int);
	      ErrorConsole err;
	      
	   public:	   	      
	      ~TransactionManager();
	      static TransactionManager* getHandle();
	   /* called at server startup: */
//	      int init(StoreManager*);
	      int init(StoreManager*, LogManager*);
	   /* executor calling: */
	      int createTransaction(Transaction* &tr);
	      int commit(Transaction*);
	      int abort(Transaction*);
	   /* log calling: */
	      list<int>* getTransactions();
	};
};

#endif
