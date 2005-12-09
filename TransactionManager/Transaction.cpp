#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <vector>

#include "Transaction.h"
#include "Semlib.h"
#include "../Errors/ErrorConsole.h"
#include "../Lock/Lock.h"


using namespace LockMgr;
using namespace Logs;
using namespace Errors;

namespace TManager
{

/*______TransactionID________________________________________*/
	TransactionID::TransactionID(int id)
	{
	    this->id = id;
	};
	int TransactionID::getId() const { return id; }

/*______Transaction_________________________________________*/
	Transaction::Transaction() 
	{
	    err = ErrorConsole(); 
	};

	Transaction::Transaction(TransactionID* tid)
	{
	    err << "Transaction started\n";
	    this->tid = tid;
	    tm = TransactionManager::getHandle();
	    lm = LockManager::getHandle();
	};

	int Transaction::getId() { return tid->getId(); };

	int Transaction::init(StoreManager *stmg, LogManager *lgmg)
	{
	    unsigned id;
	    sm = stmg;
	    logm = lgmg;
	    /* message to Logs */
	    return logm->beginTransaction(tid, id);
	}
				
	int Transaction::getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p)
	{
	    int errorNumber;
	
	    err << "Transaction: getObjectPointer\n";
	    errorNumber = lm->lock(lid, tid, mode);
	    if (errorNumber == 0)
    		errorNumber = sm->getObject( tid, lid, mode, p);

	    return errorNumber; 
	};

	int Transaction::createObject(string name, DataValue* value, ObjectPointer* &p)
	{
	    int errorNumber;
	    
	    err << "Transaction: createObject\n";
	    errorNumber = sm->createObject( tid, name, value, p);
	    
	    return errorNumber;
	}

	int Transaction::deleteObject(ObjectPointer* object)
	{
	    int errorNumber;

	    err << "Transaction: deleteObject\n";
	    /* exclusive lock for this object */
	    errorNumber = lm->lock(object->getLogicalID(), tid, Write);
	    if (errorNumber == 0)
		errorNumber = sm->deleteObject(tid, object);

	    return errorNumber;
	}


        int Transaction::getRoots(vector<ObjectPointer*>* &p)
	{
	    int errorNumber;
	    //lock?
	    errorNumber = sm->getRoots(tid, p);
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


	// Data creation
	int Transaction::createIntValue(int value, DataValue* &dataVal)
	{
	    /* temporary interface, error number returnig should be here */
	    dataVal = sm->createIntValue(value);
	    return 0;
	}

	int Transaction::createDoubleValue(double value, DataValue* &dataVal)
	{
	    /* temporary interface, error number returnig should be here */
	    dataVal = sm->createDoubleValue(value);
	    return 0;
	}
	int Transaction::createStringValue(string value, DataValue* &dataVal)
	{
	    /* temporary interface, error number returnig should be here */
	    dataVal = sm->createStringValue(value);
	    return 0;
	}

	int Transaction::createVectorValue(vector<ObjectPointer*>* value, DataValue* &dataVal)
	{
	    /* temporary interface, error number returnig should be here */
	    dataVal = sm->createVectorValue(value);
	    return 0;
	}

	int Transaction::createPointerValue(ObjectPointer* value, DataValue* &dataVal)
	{
	    /* temporary interface, error number returnig should be here */
	    dataVal = sm->createPointerValue(value);
	    return 0;
	}


	int Transaction::commit()
	{
	    int errorNumber;
	    unsigned id;
	    
	    err << "Transaction: commit\n";
	    errorNumber = logm->commitTransaction(tid, id);  //need to process error
	    return tm->commit(this);
	}

	int Transaction::abort()
	{
	    int errorNumber;
	    unsigned id;
	    
	    err << "Transaction: abort\n";
	    errorNumber = logm->rollbackTransaction(tid, id);  //need to process error
	    return tm->abort(this);
	}

	    
/*______TransactionManager______________________________________ */

	TransactionManager::TransactionManager() 
	{
	    err = ErrorConsole();
	    if ( (mutex = create_sem(SEMKEY1)) < 0 )
		 err << "TManager: cannot create sem\n";

	     V(mutex);	     
	     transactions = new list<int>;
	     printf("Mutex: %d\n", mutex);
	};
	
	TransactionManager* TransactionManager::tranMgr = new TransactionManager();
	
	TransactionManager* TransactionManager::getHandle() { return tranMgr; };
	
	TransactionManager::~TransactionManager()
	{
	    release_sem(mutex);
	}
	int TransactionManager::createTransaction(Transaction* &tr)
	{

	    P(mutex);
		int currentId = transactionId;
		transactionId++;
		addTransaction(currentId);
	    V(mutex);
	    
	    TransactionID* tid = new TransactionID(currentId);	    	
	    tr = new Transaction(tid);	
	    return tr->init(tranMgr->storeMgr, tranMgr->logMgr);
	}              

	int TransactionManager::init(StoreManager *strMgr, LogManager *logsMgr)
	{
	    storeMgr = strMgr;
	    logMgr = logsMgr;
	    return 0;
	}

	void TransactionManager::addTransaction(int id)
	{
	    transactions->push_back(id); 
	}
	
	list<int>* TransactionManager::getTransactions()
	{
	    /* block creating new transactions */
	    P(mutex);
		list<int>* copy_of_transactions = new list<int>;
		for (list<int>::iterator i = transactions->begin();
			i != transactions->end(); i++)
		copy_of_transactions->push_back(*i);
	    V(mutex);
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
	    P(mutex);
		transactions->remove(id);
	    V(mutex);
	    return 0;
	}
}
