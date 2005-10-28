#ifndef _TRANSACTION_
  #define _TRANSACTION_

class TransactionID;

#include<stdio.h>
#include "../Store/Store.h"
using namespace Store;

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
    TransactionID *tid;
    StoreManager* sm;

      public:
            Transaction(TransactionID* tId);
            int getObjectPointer(ObjectPointer& p, LogicalID& lid, AccessMode mode);
      private:
	    void setSM(StoreManager*);
	    void setID(int);
}; 

/**
 *	TransactionManager - fabryka obiektow Transaction, kazdy z Executor'ow prosi go o nowa transakcje
 */
class TransactionManager
{
    StoreManager* storeMgr;
      public:
      static int createTransaction(Transaction& tr);
      int init(StoreManager *strMgr);
};

/**
 *	LockManager - implementuje tablice zamkow, wywolywany przez dana Transakcje 
 */
class LockManager
{ 
      public:      
      static int lock(LogicalID& lid, TransactionID* tid, AccessMode mode);
};
#endif
