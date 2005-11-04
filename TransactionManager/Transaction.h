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

namespace TManager
{

	/**
	 *	TransactionID - obiekt jednoznacznie identyfikujacy transakcje
	 */
	class TransactionID 
	{      
	      public:
	      TransactionID(int n);
	};

	/**
	 *   Transaction - obiekt w ramach ktorego dziala Executor
	 */
	class Transaction
	{
	    friend class TransactionManager;
	    TransactionID *tid;
	    StoreManager* sm;

	      public:
		    Transaction();
		    Transaction(TransactionID* tId);
		    int getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p);
		    int commit();
	      private:
		    void setSM(StoreManager*);
		    void setID(int);
	}; 

	/**
	 *	TransactionManager - fabryka obiektow Transaction, kazdy z Executor'ow prosi go o nowa transakcje
	 *      Sigleton Design Pattern
	 */
	class TransactionManager
	{ 
	   private:
	      static TransactionManager *tranMgr;   	   
	      StoreManager* storeMgr;	   
	      TransactionManager();
	      
	   public:	   	      
	      static TransactionManager* getHandle();
	      int createTransaction(Transaction* &tr);
	      int init(StoreManager *strMgr);
	};


 

};

#endif
