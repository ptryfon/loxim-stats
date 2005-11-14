#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <vector>

#include "Transaction.h"
#include "Semlib.h"
#include "../Lock/Lock.h"

using namespace LockMgr;

namespace TManager
{

/*______TransactionID________________________________________*/
	TransactionID::TransactionID(int id)
	{
	    printf("TransactionID");
	    this->id = id;
	};

/*______Transaction_________________________________________*/
	Transaction::Transaction() {};

	Transaction::Transaction(TransactionID* tid)
	{
	    printf("Transaction");
	    this->tid = tid;
	    tm = TransactionManager::getHandle();
	    lm = LockManager::getHandle();
	};

	void Transaction::setSM(StoreManager* stmg)
	{
	    sm = stmg;
	};
			
	int Transaction::getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p)
	{
	    ObjectPointer *tmp;

	    printf("Transaction: getProxy\n"); fflush(stdout);
	    lm->lock(lid, tid, mode);
	    tmp = sm->getObject( tid, lid, mode );
	    p = tmp;
	    return 0; 
	};

	int Transaction::createObject(string name, DataValue* value, ObjectPointer* &p)
	{
	    ObjectPointer *tmp;

	    printf("Transaction: create\n"); fflush(stdout);
	    tmp = sm->createObject( tid, name, value );
	    p = tmp;
	    // locking object
	    return 0; 
	}

	int Transaction::deleteObject(ObjectPointer* object)
	{
	    printf("Transaction: delete\n"); fflush(stdout);
	    /* exclusive lock for this object */
	    lm->lock(object->getLogicalID(), tid, Write);
	    sm->deleteObject(tid, object);
	    
	    return 0; 
	}

	int Transaction::commit()
	{
	    printf("Transaction commited\n"); fflush(stdout);
	    return tm->commit(this);
	}

	int Transaction::abort()
	{
	    printf("Transaction aborted\n"); fflush(stdout);
	    return tm->abort(this);
	}

	    
/*______TransactionManager______________________________________ */

	TransactionManager::TransactionManager() 
	{
	    if ((mutex = semget(SEMKEY1, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0)	    
		 printf("Error in semget\n"); //exit(1); }	     
	};
        
	TransactionManager* TransactionManager::tranMgr = new TransactionManager();
	
	TransactionManager* TransactionManager::getHandle() { return tranMgr; };
	
	int TransactionManager::createTransaction(Transaction* &tr)
	{
	    printf("TransactionManager::createTransaction 0\n");fflush(stdout);
	    
	    P(mutex);
		int currentId = transactionId;
		transactionId++;
	    V(mutex);
	    
	    TransactionID* tid = new TransactionID(currentId);
	    addActiveTransaction(tid);
	    tr = new Transaction(tid);
	    tr->setSM(tranMgr->storeMgr);
	    return 0;           
	}              

	int TransactionManager::init(StoreManager *strMgr)
	{
	    printf("TransactionManager - stworzony");
	    storeMgr = strMgr;
	    return 0;
	}

	void TransactionManager::addActiveTransaction(TransactionID* tid)
	{
	    // adding new Transaction to internal data structure - hashmap 
	}
	
	vector<int>* TransactionManager::getActiveTransactions()
	{
	    vector<int>* v = new vector<int>(0);
	    /* block creating new transactions */
	    P(mutex);
		// get active transactions from internal data structure
	    V(mutex);
	    return v;  
	}
	
	int TransactionManager::commit(Transaction* tr)
	{
	    delete tr;
	    return 0;
	}
	
	int TransactionManager::abort(Transaction* tr)
	{
	    delete tr;
	    return 0;
	}
}
