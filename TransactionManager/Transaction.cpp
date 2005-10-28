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
Transaction::Transaction() {};

Transaction::Transaction(TransactionID* tId)
{
    printf("Transaction");
    tid = tId;
};

void Transaction::setSM(StoreManager* stmg)
{
    sm = stmg;
}

                
int Transaction::getObjectPointer(LogicalID& lid, AccessMode mode, ObjectPointer& p)
{
    ObjectPointer *tmp;

    printf("Transaction: getProxy");
    LockManager::lock(lid, tid, mode);
    //wywolanie Store'a
    tmp = sm->getObject( tid, &lid, mode );
    return 0; 
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
