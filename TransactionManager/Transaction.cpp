#include<stdio.h>
#include "Transaction.h"
#include "../Lock/Lock.h"

using namespace LockMgr;

namespace TManager
{

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

			
	int Transaction::getObjectPointer(LogicalID*  lid, AccessMode mode, ObjectPointer* &p)
	{
	    ObjectPointer *tmp;

	    printf("Transaction: getProxy\n"); fflush(stdout);
	    LockManager::getHandle()->lock(lid, tid, mode);
	    //wywolanie Store'a
	    tmp = sm->getObject( tid, lid, mode );
	    //p = tmp;
	    return 0; 
	};

	int Transaction::createObject(string name, DataValue* value, ObjectPointer* &p)
	{
	    ObjectPointer *tmp;

	    printf("Transaction: create\n"); fflush(stdout);
	    tmp = sm->createObject( tid, name, value );
	    // rozszerzenie zamkow
	    //LockManager::getHandle()->createLock(tmp->getLogicalID());
	    return 0; 
	}

	int Transaction::deleteObject(ObjectPointer* object)
	{
	    printf("Transaction: delete\n"); fflush(stdout);
	    // zablokowanie obiektu na wylacznosc
	    LockManager::getHandle()->lock(object->getLogicalID(), tid, Write);
	    sm->deleteObject(tid, object);
	    // skasowanie zamka
	    //LockManager::getHandle()->deleteLock(tmp->getLogicalID());
	    return 0; 
	}

	int Transaction::commit()
	{
	    printf("Transaction commited\n"); fflush(stdout);
	    return 0;
	}

	int Transaction::abort()
	{
	    printf("Transaction aborted\n"); fflush(stdout);
	    return 0;
	}



	    
	/**
	 *	TransactionManager - fabryka obiektow Transaction, kazdy z Executor'ow prosi go o nowa transakcje
	 */

	TransactionManager::TransactionManager() {}
        
	TransactionManager* TransactionManager::tranMgr = new TransactionManager();
	
	TransactionManager* TransactionManager::getHandle() { return tranMgr; };
	
	int TransactionManager::createTransaction(Transaction* &tr)
	{
	    printf("TransactionManager::createTransaction 0\n");fflush(stdout);
	    tr = new Transaction( new TransactionID(1));
	    tr->setSM(tranMgr->storeMgr);
	    return 0;           
	}              

	int TransactionManager::init(StoreManager *strMgr)
	{
	    printf("TransactionManager - stworzony");
	    storeMgr = strMgr;
	    return 0;
	}




};
