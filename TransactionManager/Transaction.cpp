#include<stdio.h>
#include "transaction.h"

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
	    LockManager::lock(lid, tid, mode);
	    //wywolanie Store'a
	    tmp = sm->getObject( tid, lid, mode );
	    return 0; 
	};


	/**
	 *	TransactionManager - fabryka obiektow Transaction, kazdy z Executor'ow prosi go o nowa transakcje
	 */

	TransactionManager::TransactionManager() {
	  if (tranMgr != NULL) {
	    fprintf(stderr, "Only one instance of TransactionManager allowed");
	    exit(1);
	  }
	}

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
	    tranMgr = this;
	    return 0;
	}
	    
	TransactionManager* TransactionManager::tranMgr = NULL;

	/**
	 *	LockManager - implementuje tablice zamkow, wywolywany przez dana Transakcje 
	 */
	int LockManager::lock(LogicalID* lid, TransactionID* tid, AccessMode mode)
	{
	    printf("LockManager\n");fflush(stdout);
	    return 0;
	};
};
