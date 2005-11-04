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

	    printf("Transaction: getProxy\n");fflush(stdout);
	    LockManager::getHandle()->lock(lid, tid, mode);
	    //wywolanie Store'a
	    tmp = sm->getObject( tid, lid, mode );
	    return 0; 
	};

	int Transaction::commit()
	{
	    printf("Transaction commited\n"); fflush(stdout);
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
