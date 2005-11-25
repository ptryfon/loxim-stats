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
	int TransactionID::getId() { return id; }

/*______Transaction_________________________________________*/
	Transaction::Transaction() {};

	Transaction::Transaction(TransactionID* tid)
	{
	    printf("Transaction");
	    this->tid = tid;
	    tm = TransactionManager::getHandle();
	    lm = LockManager::getHandle();
	};

	int Transaction::getId() { return tid->getId(); };

	void Transaction::setSM(StoreManager* stmg)
	{
	    sm = stmg;
	};
			
	int Transaction::getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p)
	{
	    int errorNumber;

	    printf("Transaction: getProxy\n"); fflush(stdout);
	    errorNumber = lm->lock(lid, tid, mode);
	    if (errorNumber == 0)
    		errorNumber = sm->getObject( tid, lid, mode, p);

	    return errorNumber; 
	};

	int Transaction::createObject(string name, DataValue* value, ObjectPointer* &p)
	{
	    int errorNumber;

	    printf("Transaction: create\n"); fflush(stdout);
	    errorNumber = sm->createObject( tid, name, value, p);
	    //p = tmp;
	    // locking object
	    return errorNumber;
	}

	int Transaction::deleteObject(ObjectPointer* object)
	{
	    int errorNumber;

	    printf("Transaction: delete\n"); fflush(stdout);
	    /* exclusive lock for this object */
	    errorNumber = lm->lock(object->getLogicalID(), tid, Write);
	    if (errorNumber == 0)
		errorNumber = sm->deleteObject(tid, object);

	    return errorNumber;
	}


        int Transaction::getRoots(vector<ObjectPointer*>* &p)
	{
	    int errorNumber;

	    errorNumber = sm->getRoots(tid, p);
	    //lock?
	    return errorNumber;
	}

	int Transaction::getRoots(string name, vector<ObjectPointer*>* &p)
	{
	    int errorNumber;

	    errorNumber = sm->getRoots(tid, name, p);
	    //lock?
	    return errorNumber;
	}

	int Transaction::addRoot(ObjectPointer* &p)
	{
	    int errorNumber;

	     errorNumber = sm->addRoot(tid, p);
	    //lock?
	    return errorNumber;
	}

	int Transaction::removeRoot(ObjectPointer* &p)
	{
	    int errorNumber;

	    errorNumber = sm->addRoot(tid, p);
	    //lock?
	    return errorNumber;
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
	    //if ((mutex = semget(SEMKEY1, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0)	    
	//	 printf("Error in semget\n"); 
	//    printf("Mutex created\n");
	//	 V(mutex);
	//    printf("V mutex\n");
	    transactions = new list<int>;
	};
        
	TransactionManager* TransactionManager::tranMgr = new TransactionManager();
	
	TransactionManager* TransactionManager::getHandle() { return tranMgr; };
	
	TransactionManager::~TransactionManager()
	{
	//      semctl (mutex, 0, IPC_RMID,0);
              	    
	}
	int TransactionManager::createTransaction(Transaction* &tr)
	{
	    printf("TransactionManager::createTransaction\n");fflush(stdout);
	    
	//    P(mutex);
	        printf("Sekcja krytyczna \n");
		int currentId = transactionId;
		transactionId++;
		addTransaction(currentId);
	//    V(mutex);
	    
	    TransactionID* tid = new TransactionID(currentId);	    
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

	void TransactionManager::addTransaction(int id)
	{
	    transactions->push_back(id); 
	}
	
	list<int>* TransactionManager::getTransactions()
	{
	    /* block creating new transactions */
	//    P(mutex);
		list<int>* copy_of_transactions = new list<int>;
		for (list<int>::iterator i = transactions->begin();
			i != transactions->end(); i++)
		copy_of_transactions->push_back(*i);
	//    V(mutex);
	    return copy_of_transactions;  
	}
	
	int TransactionManager::commit(Transaction* tr)
	{
	    remove_from_list(tr->getId());
	    delete tr;
	    return 0;
	}
	
	int TransactionManager::abort(Transaction* tr)
	{
	    remove_from_list(tr->getId());	
	    delete tr;
	    return 0;
	}

	int TransactionManager::remove_from_list(int id)
	{
	//    P(mutex);
		transactions->remove(id);
	//    V(mutex);
	    return 0;
	}
}
