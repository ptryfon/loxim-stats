
#include "Transaction.h"

namespace TManager
{

/*______TransactionID________________________________________*/
	TransactionID::TransactionID(int id)
	{
		this->id = id;
	};
	int TransactionID::getId() const { return id; };

	TransactionID* TransactionID::clone()
	{
		return new TransactionID(this->id);
	}
	
/*______Transaction_________________________________________*/
	Transaction::Transaction(TransactionID* tid, Semaphore* _sem)
	{
		err = ErrorConsole("TransactionManager"); 
		err.printf("Transaction started, id: %d\n", tid->getId());	
		sem = _sem;
		this->tid = tid;
		tm = TransactionManager::getHandle();
		lm = LockManager::getHandle();	    	    
	};
	
	Transaction::~Transaction()
	{
		delete tid;
	}
	
	TransactionID* Transaction::getId() { return tid; };

	int Transaction::init(StoreManager *stmg, LogManager *lgmg)
	{
		unsigned id;
		sm = stmg;
		logm = lgmg;
		
		/* message to Logs */
	    	return logm->beginTransaction(tid->getId(), id);
	}
				
	int Transaction::getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p)
	{
		int errorNumber;
	
		err.printf("Transaction: %d getObjectPointer, mode %d\n", tid->getId(), mode);
		errorNumber = lm->lock(lid, tid, mode);

		if (errorNumber == 0)
		{
		    sem->lock_read();
		    
		    	errorNumber = sm->getObject( tid, lid, mode, p);
			
		    sem->unlock();
		}
		
		if (errorNumber) abort();
    
		return errorNumber; 
	}
	
	int Transaction::modifyObject(ObjectPointer*& op, DataValue* dv)
	{
	    int errorNumber;

//	    DBPhysicalID oldId = (*((op->getLogicalID())->getPhysicalID()));

	    err.printf("Transaction: %d modifyObject\n", tid->getId());
	    
	    errorNumber = lm->lock(op->getLogicalID(), tid, Write);

	    if (errorNumber == 0)
	    {
		sem->lock_write();

		    errorNumber = sm->modifyObject(tid, op, dv);

		    if (errorNumber == 0)
			errorNumber = lm->lock(op->getLogicalID(), tid, Write);
		    /* we want new object to be seen as old with new value */
//			errorNumber = lm->modifyLockLID(&oldId, op->getLogicalID(), tid);
		    /* in case of error old object would be released after abort() */

		sem->unlock();
	    }

	    if (errorNumber) abort();

	    return errorNumber;
	}
	
	int Transaction::createObject(string name, DataValue* value, ObjectPointer* &p)
	{
		int errorNumber;
	    
		err.printf("Transaction: %d createObject\n", tid->getId());

		sem->lock_write();
			errorNumber = sm->createObject( tid, name, value, p);
			
			if (errorNumber == 0)
			/* quaranteed not wait for lock */			
			    errorNumber = lm->lock(p->getLogicalID(), tid, Write);
			    
		sem->unlock();
		
		if (errorNumber) abort();
		    
		return errorNumber;
	};

	int Transaction::deleteObject(ObjectPointer* object)
	{
		int errorNumber;

		err.printf("Transaction: %d deleteObject\n", tid->getId());
		/* exclusive lock for this object */
		errorNumber = lm->lock(object->getLogicalID(), tid, Write);
		
		if (errorNumber == 0)
		{		
		    sem->lock_write();				
		    		    
			errorNumber = sm->deleteObject(tid, object);		
		    		    
		    sem->unlock();
		}
		
		if (errorNumber) abort();
		    
		return errorNumber;
	}


    	int Transaction::getRoots(vector<ObjectPointer*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getRoots\n", tid->getId());
		
		sem->lock_read();
			errorNumber = sm->getRoots(tid, p);			
		sem->unlock();
		
		if (errorNumber == 0)
		{
		    for (vector<ObjectPointer*>::iterator iter = p->begin();
	    	        iter != p->end(); iter++ ) 
		    {     
			errorNumber = lm->lock( (*iter)->getLogicalID(), tid, Read);
		    		    
			if (errorNumber) 
			{
			    abort(); break;
			}
		    }
		}	
		return errorNumber;
	}

	int Transaction::getRoots(string name, vector<ObjectPointer*>* &p)
	{
		int errorNumber;
		
		err.printf("Transaction: %d getRoots by name \n", tid->getId());

		sem->lock_read();
			errorNumber = sm->getRoots(tid, name, p);
		sem->unlock();
		
		if (errorNumber == 0)
		{
		    for (vector<ObjectPointer*>::iterator iter = p->begin();
	    	        iter != p->end(); iter++ ) 
		    {     
			errorNumber = lm->lock( (*iter)->getLogicalID(), tid, Read);
			
			if (errorNumber) 
			{
			    abort(); break;
			}
		    }
		}
		return errorNumber;
	}

	int Transaction::addRoot(ObjectPointer* &p)
	{
		int errorNumber;
		
		err.printf("Transaction: %d addRoot\n", tid->getId());
		
		sem->lock_write();
			errorNumber = sm->addRoot(tid, p);
			/* GUARANTEED no waiting */
			if (errorNumber == 0)
			    errorNumber = lm->lock( p->getLogicalID(), tid, Write);
		sem->unlock();
		
		if (errorNumber) abort();
			    
		return errorNumber;
	}

	int Transaction::removeRoot(ObjectPointer* &p)
	{
		int errorNumber;
		
		err.printf("Transaction: %d removeRoot\n", tid->getId());
		
		errorNumber = lm->lock( p->getLogicalID(), tid, Write);
		
		if (errorNumber == 0)
		{
		    sem->lock_write();
			errorNumber = sm->removeRoot(tid, p);		
		    sem->unlock();
		}
				
		if (errorNumber) abort();
			    	   
		return errorNumber;
	}


	/* Data creation */
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

	int Transaction::createVectorValue(vector<LogicalID*>* value, DataValue* &dataVal)
	{
		/* temporary interface, error number returnig should be here */
		sem->lock_write();
			dataVal = sm->createVectorValue(value);
		sem->unlock();
		return 0;
	}

	int Transaction::createPointerValue(LogicalID* value, DataValue* &dataVal)
	{
		/* temporary interface, error number returnig should be here */
		sem->lock_write();
			dataVal = sm->createPointerValue(value);
		sem->unlock();
		return 0;
	}
	
	int Transaction::commit()
	{	
	    err.printf("Transaction commit, tid = %d\n", tid->getId());
	    int retval = tm->commit(this);
	    sm->commitTransaction(tid);
	    return retval;
	}

	int Transaction::abort()
	{	
	    err.printf("Transaction abort, tid = %d\n", tid->getId());
	    int retval = tm->abort(this);
	    sm->abortTransaction(tid);
	    return retval;
	}

	    
/*______TransactionManager______________________________________ */

	TransactionManager::TransactionManager() 
	{
		err = ErrorConsole("TransactionManager");
		sem = new RWUJSemaphore();
		sem->init();
		mutex = new Mutex();
		mutex->init();
		transactions = new list<TransactionID*>;
	};
	
	TransactionManager* TransactionManager::tranMgr = new TransactionManager();
	
	TransactionManager* TransactionManager::getHandle() { return tranMgr; };
	
	TransactionManager::~TransactionManager()
	{
		err.printf("Niszcze TransactionManagera\n");
		/* unlock all unfinished transactions */
		for (list<TransactionID*>::iterator i = transactions->begin();	i != transactions->end(); i++)
		    LockManager::getHandle()->unlockAll(*i);
		delete sem;
		delete mutex;
		delete transactions;
		
		delete LockManager::getHandle();
	}

	int TransactionManager::createTransaction(Transaction* &tr)
	{
	    err.printf("Creating new transaction\n");
	    mutex->down();
			int currentId = transactionId;
			transactionId++;
			TransactionID* tid = new TransactionID(currentId);	    	
			addTransaction(tid);
			err.printf("Transaction created -> number %d\n", tid->getId());
	    mutex->up();
	    	    
	    tr = new Transaction(tid, sem);	
	    return tr->init(storeMgr, logMgr);
	}              

	int TransactionManager::init(StoreManager *strMgr, LogManager *logsMgr)
	{
	    err.printf("Transaction Manager starting .... ");
	    storeMgr = strMgr;
	    logMgr = logsMgr;
	    err.printf("DONE\n");
	    return 0;
	}

	void TransactionManager::addTransaction(TransactionID* tid)
	{
	    transactions->push_back(tid); 
	}	
	list<TransactionID*>* TransactionManager::getTransactions()
	{
	    /* block creating new transactions */
	    mutex->down();
			list<TransactionID*>* copy_of_transactions = new list<TransactionID*>;
			for (list<TransactionID*>::iterator i = transactions->begin();	
				i != transactions->end(); i++)
				
				copy_of_transactions->push_back((*i)->clone());
	    mutex->up();
	    return copy_of_transactions;  
	}	
	vector<int>* TransactionManager::getTransactionsIds()
	{
	    /* block creating new transactions */
	    mutex->down();
			vector<int>* copy_of_transactions = new vector<int>;
			for (list<TransactionID*>::iterator i = transactions->begin();	
				i != transactions->end(); i++)
				
				copy_of_transactions->push_back((*i)->getId());
	    mutex->up();
	    return copy_of_transactions;  
	}	
	int TransactionManager::commit(Transaction* tr)
	{
		int errorNumber;		
		unsigned id;
		
		/* commit record in logs*/
		errorNumber = logMgr->commitTransaction(tr->getId()->getId(), id); 
		err.printf("Transaction commit, tid = %d, logs errno = %d\n", tr->getId()->getId(), errorNumber);
		
		/* unlock all objects hold by transaction */
		errorNumber = LockManager::getHandle()->unlockAll(tr->getId());
		err.printf("Transaction commit, tid = %d, lock errno = %d\n", tr->getId()->getId(), errorNumber);
		
		/* remove transaction from active transactions list */
	    	remove_from_list(tr->getId());
		
		/* free memory */
	    	delete tr;
		
	    	return errorNumber;
	}	
	int TransactionManager::abort(Transaction* tr)
	{		
		int errorNumber;		
		unsigned id;
		
		/* rollback record in logs plus perform rollback operation */
		errorNumber = logMgr->rollbackTransaction(tr->getId()->getId(), storeMgr, id); 
		err.printf("Transaction abort, tid = %d, logs errno = %d\n", tr->getId()->getId(), errorNumber);
		
		/* unlock all objects hold by transaction */
		errorNumber = LockManager::getHandle()->unlockAll(tr->getId());
		err.printf("Transaction abort, tid = %d, lock errno = %d\n", tr->getId()->getId(), errorNumber);
		
		/* remove transaction from active transactions list */
	    	remove_from_list(tr->getId());
		
		/* free memory */
	    	delete tr;
		
	    	return errorNumber;
	}
	
	int TransactionManager::remove_from_list(TransactionID* tid)
	{
	    	mutex->down();
			transactions->remove(tid);
	    	mutex->up();
	    	return 0;
	}
}
