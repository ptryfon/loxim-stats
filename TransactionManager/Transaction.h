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

      public:
            Transaction(TransactionID* tId);
            ObjectPointer* getObjectPointer(ObjectPointer& p, LogicalID& lid, ObjectPointer::AccessMode mode);
}; 

/**
 *	TransactionManager - fabryka obiektow Transaction, kazdy z Executor'ow prosi go o nowa transakcje
 */
class TransactionManager
{
      public:
      static int createTransaction(Transaction& tr);
};

/**
 *	LockManager - implementuje tablice zamkow, wywolywany przez dana Transakcje 
 */
class LockManager
{ 
      public:      
      static int lock(LogicalID& lid, TransactionID* tid, ObjectPointer::AccessMode mode);
};
#endif
