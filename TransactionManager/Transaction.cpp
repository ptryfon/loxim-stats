#include<stdio.h>
#include "transaction.h"
/**
 *	TransactionID - obiekt jednoznacznie identyfikujacy transakcje
 */
TransactionID::TransactionID(int id)
{
    printf("TransactionID");
};

/**
 *   Transaction - obiekt w ramach ktorego dziala Executor
 */
Transaction::Transaction(TransactionID* tId)
{
    printf("Transaction");
    tid = tId;
};
                
int Transaction::getObjectPointer(ObjectPointer& p,LogicalID& lid, AccessMode mode)
{
    printf("Transaction: getProxy");
    LockManager::lock(lid, tid, mode);
    return 0; // StoreManager::getObject( &tid, &lid, mode );
};


/**
 *	TransactionManager - fabryka obiektow Transaction, kazdy z Executor'ow prosi go o nowa transakcje
 */
int TransactionManager::createTransaction(Transaction& tr)
{
    printf("TransactionManager");
    //tr = new Transaction( new TransactionID(1));
     return 0;           
}              

int TransactionManager::init(StoreManager *strMgr)
{
    printf("TransactionManager - stworzony");
    storeMgr = strMgr;
    return 0;
}
    

/**
 *	LockManager - implementuje tablice zamkow, wywolywany przez dana Transakcje 
 */
int LockManager::lock(LogicalID& lid, TransactionID* tid, AccessMode mode)
{
    printf("LockManager");
    return 0;
};
