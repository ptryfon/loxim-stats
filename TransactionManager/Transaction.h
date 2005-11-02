#ifndef _TRANSACTION_
#define _TRANSACTION_

namespace TManager
{
	class TransactionID;
};

#include<stdio.h>
#include "../Store/Store.h"
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
	   StoreManager* storeMgr;
	   static TransactionManager *tranMgr;   
	   public:
	      TransactionManager();
	      static int createTransaction(Transaction* &tr);
	      int init(StoreManager *strMgr);
	};

	/**
	 *	LockManager - implementuje tablice zamkow, wywolywany przez dana Transakcje 
	 */
	class LockManager
	{ 
	      public:      
	      static int lock(LogicalID* lid, TransactionID* tid, AccessMode mode);
	};

};

#endif
