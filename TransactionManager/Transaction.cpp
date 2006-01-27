
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
	
		err << "Transaction: getObjectPointer";
		errorNumber = lm->lock(lid, tid, mode);

		sem->lock_read();

			if (errorNumber == 0)
    				errorNumber = sm->getObject( tid, lid, mode, p);
		sem->unlock();

		return errorNumber; 
	};

	int Transaction::createObject(string name, DataValue* value, ObjectPointer* &p)
	{
		int errorNumber;
	    
		err << "Transaction: createObject";

		sem->lock_write();
			errorNumber = sm->createObject( tid, name, value, p);
			
			if (errorNumber == 0)
			/* quaranteed not wait for lock */			
			    errorNumber = lm->lock(p->getLogicalID(), tid, Write);
			    
		sem->unlock();

		return errorNumber;
	};

	int Transaction::deleteObject(ObjectPointer* object)
	{
		int errorNumber;

		err << "Transaction: deleteObject";
		/* exclusive lock for this object */
		errorNumber = lm->lock(object->getLogicalID(), tid, Write);
		
		sem->lock_write();		
		
		err << "Before store delete";
			if (errorNumber == 0)
				errorNumber = sm->deleteObject(tid, object);		
		err << "After store delete";
		sem->unlock();
		
		return errorNumber;
	}


    	int Transaction::getRoots(vector<ObjectPointer*>* &p)
	{
		int errorNumber;

		sem->lock_read();
			errorNumber = sm->getRoots(tid, p);			
		sem->unlock();
		
		for (vector<ObjectPointer*>::iterator iter = p->begin();
	    	     iter != p->end(); iter++ ) 
		{     
		    int ret = lm->lock( (*iter)->getLogicalID(), tid, Read);
		    errorNumber = ret;
		}
			
		return errorNumber;
	}

	int Transaction::getRoots(string name, vector<ObjectPointer*>* &p)
	{
		int errorNumber;

		sem->lock_read();
			errorNumber = sm->getRoots(tid, name, p);
		sem->unlock();
		
		for (vector<ObjectPointer*>::iterator iter = p->begin();
	    	     iter != p->end(); iter++ ) 
		{     
		    int ret = lm->lock( (*iter)->getLogicalID(), tid, Read);
		    errorNumber = ret;
		}
		return errorNumber;
	}

	int Transaction::addRoot(ObjectPointer* &p)
	{
		int errorNumber;
		
		sem->lock_write();
			errorNumber = sm->addRoot(tid, p);
			if (errorNumber == 0)	
			    errorNumber = lm->lock( p->getLogicalID(), tid, Write);
		sem->unlock();
			    
		return errorNumber;
	}

	int Transaction::removeRoot(ObjectPointer* &p)
	{
		int errorNumber;
		
		errorNumber = lm->lock( p->getLogicalID(), tid, Write);
		
		sem->lock_write();		
			errorNumber = sm->removeRoot(tid, p);		
		sem->unlock();
			   
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
		int errorNumber;
		unsigned id;
	    
		err << "Transaction: commit";
		errorNumber = logm->commitTransaction(tid->getId(), id);  //need to process error
		return tm->commit(this);
	}

	int Transaction::abort()
	{
		int errorNumber;
		unsigned id;
	    
		err << "Transaction: abort";
		errorNumber = logm->rollbackTransaction(tid->getId(), sm, id);  //need to process error
		return tm->abort(this);
	}

	    
/*______TransactionManager______________________________________ */

	TransactionManager::TransactionManager() 
	{
		err = ErrorConsole();
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
		delete sem;
		delete mutex;
		delete transactions;
	}

	int TransactionManager::createTransaction(Transaction* &tr)
	{
	    mutex->down();
			int currentId = transactionId;
			transactionId++;
			TransactionID* tid = new TransactionID(currentId);	    	
			addTransaction(tid);
	    mutex->up();
	    	    
	    tr = new Transaction(tid, sem);	
	    return tr->init(tranMgr->storeMgr, tranMgr->logMgr);
	}              

	int TransactionManager::init(StoreManager *strMgr, LogManager *logsMgr)
	{
	    storeMgr = strMgr;
	    logMgr = logsMgr;
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
			for (list<TransactionID*>::iterator i = transactions->begin();	i != transactions->end(); i++)
				copy_of_transactions->push_back((*i)->clone());
	    mutex->up();
	    return copy_of_transactions;  
	}	
	vector<int>* TransactionManager::getTransactionsIds()
	{
	    /* block creating new transactions */
	    mutex->down();
			vector<int>* copy_of_transactions = new vector<int>;
			for (list<TransactionID*>::iterator i = transactions->begin();	i != transactions->end(); i++)
				copy_of_transactions->push_back((*i)->getId());
	    mutex->up();
	    return copy_of_transactions;  
	}	
	int TransactionManager::commit(Transaction* tr)
	{
		int errorNumber;
		errorNumber = LockManager::getHandle()->unlockAll(tr->getId());
	    	remove_from_list(tr->getId());
	    	delete tr;
	    	return errorNumber;
	}	
	int TransactionManager::abort(Transaction* tr)
	{
	    	remove_from_list(tr->getId());	
		int errorNumber;
		// handle rollback operation in colaboration with LogManager
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
