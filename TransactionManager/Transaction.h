#ifndef _TRANSACTION_
#define _TRANSACTION_

namespace TManager
{
	class TransactionID;
};

#include<stdio.h>
#include "../Store/Store.h"
#include "../Lock/Lock.h"
using namespace Store;
using namespace LockMgr;

namespace TManager
{

	/**
	 *	TransactionID - unique object for transaction
	 */
	class TransactionID 
	{      
	      private:
	        int id;
		
	      public:
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
		TransactionManager* tm;
		LockManager* lm;
		
	      public:
		    Transaction();
		    Transaction(TransactionID* tId);
	      /* Executor calls: */
		    int getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p);
		    int createObject(string name, DataValue* value, ObjectPointer* &p);
		    int deleteObject(ObjectPointer* object);

		    int abort();
		    int commit();

	      private:
		    void setSM(StoreManager*);
	}; 

	/**
	 *	TransactionManager - factory of Transactions
	 *      Sigleton Design Pattern
	 */
	class TransactionManager
	{ 
	   private:
	      int mutex;	    /* critical section */
	      int transactionId;    /* counter of TransactionID objects */	
	      static TransactionManager *tranMgr;   	   
	      StoreManager* storeMgr;	   
	      TransactionManager();
    	      void addActiveTransaction(TransactionID*);
	      
	   public:	   	      
	      static TransactionManager* getHandle();
	   /* called at server startup: */
	      int init(StoreManager *strMgr);
	   /* executor calling: */
	      int createTransaction(Transaction* &tr);
	      int commit(Transaction*);
	      int abort(Transaction*);
	   /* log calling: */
	      vector<int>* getActiveTransactions();
	};
};

#endif
