
#include <TransactionManager/Transaction.h>
#include <Indexes/IndexManager.h>
#include <TypeCheck/TypeChecker.h>
#include <SystemStats/AllStats.h>

using namespace SystemStatsLib;
/**
 *	@author Julian Krzemiski (julian.krzeminski@students.mimuw.edu.pl)
 *	@author Dominik Klimczak (dominik.klimczak@students.mimuw.edu.pl)
 */
namespace TManager
{

/*______TransactionID________________________________________*/
	TransactionID::TransactionID(int sessionId, int id, int* nothing)
	{
		this->sessionId = 0;
		this->id = id;
		this->priority = id;
	};
	TransactionID::TransactionID(int sessionId, int id, int priority)
	{
		this->sessionId = sessionId;
		this->id = id;
		this->priority = priority;
	};
	int TransactionID::getId() const { return id; };
	int TransactionID::getSessionId() const { return sessionId; };
	int TransactionID::getPriority() const { return priority; };
	unsigned TransactionID::getTimeStamp() const { return timeStamp; };
	void TransactionID::setTimeStamp(unsigned t) { timeStamp = t; };

	TransactionID* TransactionID::clone()
	{
		return new TransactionID(this->sessionId, this->id, this->priority);
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
		dmlStructs = new DMLControl(this);
	};

	Transaction::~Transaction()
	{
		delete tid;
		delete dmlStructs;
	}

	TransactionID* Transaction::getId() { return tid; };

	DMLControl *Transaction::getDmlStct() {return dmlStructs;}

	int Transaction::init(StoreManager *stmg, LogManager *lgmg)
	{
		unsigned id;
		int errorNumer;
		sm = stmg;
		logm = lgmg;

		/* message to Logs */
	    	errorNumer = logm->beginTransaction(tid->getId(), id);
		tid->setTimeStamp(id);
		dmlStructs->init(tid->getSessionId());
		//cout << dmlStructs->depsToString() << endl;
		//cout << dmlStructs->rootMdnsToString();
		return errorNumer;
	}

	void Transaction::reloadDmlStct() {
		if (dmlStructs != NULL) delete dmlStructs;
		dmlStructs = new DMLControl(this);
		dmlStructs->init(tid->getSessionId());
		cout << dmlStructs->depsToString() << endl;
		cout << dmlStructs->rootMdnsToString();
	}

	int Transaction::getObjectPointer(LogicalID* lid, AccessMode mode, ObjectPointer* &p, bool allowNullObject)
	{
		int errorNumber;

		err.printf("Transaction: %u getObjectPointer, mode %d\n", tid->getId(), mode);
		if(lid->toInteger() & 0xFF000000) {
			err.printf("TransactionStore::getObject from systemViews LID = %u\n", lid->toInteger());
			/* Obiekty widoku systemowego, gdy zapalone są bity 31-24*/
			sem->lock_read();

			errorNumber = sm->getObject( tid, lid, mode, p);

			sem->unlock();
		} else {
			errorNumber = lm->lock(lid, tid, mode);

			if (errorNumber == 0)
			{
				sem->lock_read();

					errorNumber = sm->getObject( tid, lid, mode, p);

				sem->unlock();
			}

		}

		if (errorNumber) {
			abort();
			return errorNumber;
		}

		if (p == NULL && !allowNullObject) {
			err.printf("getObjectPointer, object doesn't exist while we expect existing object\n");
			errorNumber = ENoObject | ErrTManager;
			abort();
		}

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

			//po modyfikacji w storze *op bedzie juz innym obiektem o tym samym lid
			errorNumber = Indexes::IndexManager::getHandle()->modifyObject(tid, op, dv);

			if (errorNumber == 0)
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

			/* INDEX	przechwytywanie createObject chyba nie bedzie potrzebne. zamiast tego jest addRoot
			if (errorNumber == 0)
				errorNumber = Indexes::IndexManager::getHandle()->createObject(tid, name, value, p, p->getLogicalID());
			*/

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

		    errorNumber = Indexes::IndexManager::getHandle()->deleteObject(object);

		    if (!errorNumber) {

		    	errorNumber = sm->deleteObject(tid, object);

		    }

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
		else abort();
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
		else abort();
		return errorNumber;
	}

	int Transaction::getRootsLID(vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getRootsLID\n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getRootsLID(tid, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::getRootsLID(string name, vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getRootsLID by name \n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getRootsLID(tid, name, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::getRootsLIDWithBegin(string nameBegin, vector<LogicalID*>* &p) {
		int errorNumber;

		err.printf("Transaction: %d getRootsLID by nameBegin \n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getRootsLIDWithBegin(tid, nameBegin, p);
		sem->unlock();

		if (errorNumber) abort();

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
				errorNumber = Indexes::IndexManager::getHandle()->addRoot(tid, p);

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

		    if (errorNumber == 0)
		    	errorNumber = Indexes::IndexManager::getHandle()->removeRoot(tid, p);
		}

		if (errorNumber) abort();

		return errorNumber;
	}


	int Transaction::getViewsLID(vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getViewsLID\n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getViewsLID(tid, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::getViewsLID(string name, vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getViewsLID by name \n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getViewsLID(tid, name, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}


	int Transaction::addView(const char* name, ObjectPointer* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d addView\n", tid->getId());

		sem->lock_write();
			errorNumber = sm->addView(tid, name,  p);
			/* GUARANTEED no waiting */
			if (errorNumber == 0)
			    errorNumber = lm->lock( p->getLogicalID(), tid, Write);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::removeView(ObjectPointer* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d removeView\n", tid->getId());

		errorNumber = lm->lock( p->getLogicalID(), tid, Write);

		if (errorNumber == 0)
		{
		    sem->lock_write();
			errorNumber = sm->removeView(tid, p);
		    sem->unlock();
		}

		if (errorNumber) abort();

		return errorNumber;
	}


	//classes begin

	int Transaction::getClassesLID(vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getClassesLID\n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getClassesLID(tid, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::getClassesLID(string name, vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getClassesLID by name \n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getClassesLID(tid, name, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::getClassesLIDByInvariant(string invariantName, vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getClassesLID by name \n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getClassesLIDByInvariant(tid, invariantName, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;

	}


	int Transaction::addClass(const char* name, const char* invariantName, ObjectPointer* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d addClass\n", tid->getId());

		sem->lock_write();
			errorNumber = sm->addClass(tid, name, invariantName,  p);
			/* GUARANTEED no waiting */
			if (errorNumber == 0)
			    errorNumber = lm->lock( p->getLogicalID(), tid, Write);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::removeClass(ObjectPointer* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d removeClass\n", tid->getId());

		errorNumber = lm->lock( p->getLogicalID(), tid, Write);

		if (errorNumber == 0)
		{
		    sem->lock_write();
			errorNumber = sm->removeClass(tid, p);
		    sem->unlock();
		}

		if (errorNumber) abort();

		return errorNumber;
	}


	//classes end

	//Interfaces
	int Transaction::getInterfacesLID(vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getInterfacesLID\n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getInterfacesLID(tid, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::getInterfacesLID(string name, vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getInterfacesLID by name \n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getInterfacesLID(tid, name, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	//System Views
	int Transaction::getSystemViewsLID(vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getInterfacesLID\n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getSystemViewsLID(tid, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::getSystemViewsLID(string name, vector<LogicalID*>* &p)
	{
		int errorNumber;

		err.printf("Transaction: %d getInterfacesLID by name \n", tid->getId());

		sem->lock_read();
		errorNumber = sm->getSystemViewsLID(tid, name, p);
		sem->unlock();

		if (errorNumber) abort();

		return errorNumber;
	}

	int Transaction::addInterface(const string& name, const string& objectName, ObjectPointer* &p)
	{
		int errorNumber;
		err.printf("Transaction: %d addInterface\n", tid->getId());

		sem->lock_write();
			errorNumber = sm->addInterface(tid, name, objectName, p);
			if (errorNumber == 0)
			    errorNumber = lm->lock( p->getLogicalID(), tid, Write);
		sem->unlock();

		if (errorNumber) abort();
		return errorNumber;
	}
	
	int Transaction::bindInterface(const string& name, const string& bindName)
	{
		int errorNumber;
		err.printf("Transaction: %d bindInterface\n", tid->getId());

		sem->lock_write();
			errorNumber = sm->bindInterface(tid, name, bindName);
		sem->unlock();

		if (errorNumber) abort();
		return 0;
	}
	
	int Transaction::getInterfaceBindForObjectName(const string& objectName, string& interfaceName, string& bindName)
	{
		int errorNumber;
		err.printf("Transaction: %d getInterfaceBindForObjectName\n", tid->getId());

		sem->lock_read();
			errorNumber = sm->getInterfaceBindForObjectName(tid, objectName, interfaceName, bindName);
		sem->unlock();

		if (errorNumber && (errorNumber != (ENoInterfaceFound | ErrStore))) abort();
		return errorNumber;
	}
	

	int Transaction::removeInterface(ObjectPointer* &p)
	{
		int errorNumber;
		err.printf("Transaction: %d removeInterface\n", tid->getId());
		errorNumber = lm->lock( p->getLogicalID(), tid, Write);

		if (errorNumber == 0)
		{
		    sem->lock_write();
			errorNumber = sm->removeInterface(tid, p);
		    sem->unlock();
		}

		if (errorNumber) abort();
		return errorNumber;
	}

	//interfaces end


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
	    return tm->commit(this);
	}

	int Transaction::abort()
	{
	    //err.printf("Transaction abort, tid = %d\n", tid->getId());
	    //err.printf("Transaction abort\n");
	    return tm->abort(this);
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
	 }

	 int TransactionManager::getReaderTimeout() { return readerTimeout; }
	 int TransactionManager::getWriterTimeout() { return writerTimeout; }

	 int TransactionManager::loadConfig()
	 {
		string semTime = "off";
		SBQLConfig conf("TransactionManager");
		minimalTransactionId = 1;
		semaphoresTimeout = false;
		readerTimeout = 30;
		writerTimeout = 40;
		boostAfterDeadlock = 10;

		int errorCode = conf.getInt( "minimal_transaction_id", minimalTransactionId);
		if (errorCode)
		    err.printf("Cannot read minimal_transaction_id from configuration, using default value %d\n", minimalTransactionId);
		errorCode  = conf.getString( "semaphores_timeout", semTime);
		if (semTime == "on")
		    semaphoresTimeout = true;
		if (errorCode)
		    err << "Cannot read semaphores_timeout from configuration, using default value " + semTime;
		errorCode  = conf.getInt( "reader_timeout", readerTimeout);
		if (errorCode)
		    err.printf("Cannot read reader_timout from configuration, using default value %d\n", readerTimeout);
		errorCode  = conf.getInt( "writer_timeout", writerTimeout);
		if (errorCode)
		    err.printf("Cannot read writer_timeout from configuration, using default value %d\n", writerTimeout);
		errorCode  = conf.getInt( "boost_after_deadlock", boostAfterDeadlock);
		if (errorCode)
		    err.printf("Cannot read boost_after_deadlock from configuration, using default value %d\n", boostAfterDeadlock);
/*		err.printf("First trans %i\n", minimalTransactionId);
		err << "Semaphores timeout " + semTime;
		err.printf("Reader timeout %i\n", readerTimeout);
		err.printf("Writer timeout %i\n", writerTimeout);*/

		//zeby leniwe czyszczenie indeksow dzialalo transactionId nie moga sie powtarzac po restarcie systemu
		//transactionId = minimalTransactionId - 1;
		transactionId = LogRecord::getIdSeq();

		return 0;
	 }

	TransactionManager* TransactionManager::tranMgr = NULL;//new TransactionManager();

	TransactionManager* TransactionManager::getHandle() { return tranMgr; };

	TransactionManager::~TransactionManager()
	{
		err.printf("Destroying TransactionManager\n");
		/* unlock all unfinished transactions */
		for (list<TransactionID*>::iterator i = transactions->begin();	i != transactions->end(); i++)
		    LockManager::getHandle()->unlockAll(*i);
		delete sem;
		delete mutex;
		delete transactions;

		delete LockManager::getHandle();
	}

	/**
	 * standard begining of new transaction
	 * @param tr - created transaction
	 */
	int TransactionManager::createTransaction(int sessionId, Transaction* &tr)
	{
	    return createTransaction(sessionId, tr, -1);
	}

	/**
	 * begin transaction that was aborted by deadlock
	 * @param tr created transaction
	 * @param id id of old aborted transaction
	 */
	int TransactionManager::createTransaction(int sessionId, Transaction* &tr, int id)
	{
	    err.printf("Creating new transaction\n");
	    mutex->down();
			int currentId = transactionId;
			transactionId++;
			TransactionID* tid;
			if (id > -1)
			    tid = new TransactionID(sessionId, currentId, id);
			else
			    tid = new TransactionID(sessionId, currentId, (int*)NULL);
			addTransaction(tid);
			IndexManager::getHandle()->begin(currentId);
			err.printf("Transaction created -> number %d prio: %d\n", tid->getId(), tid->getPriority());
	    mutex->up();

	    tr = new Transaction(tid, sem);
	    int ret = tr->init(storeMgr, logMgr);
	    if (ret == 0) {
	    	AllStats::getHandle()->getTransactionsStats()->startTransaction(tid->getId());
	    }
	    return ret;
	}

	int TransactionManager::init(StoreManager *strMgr, LogManager *logsMgr)
	{
		tranMgr = new TransactionManager();
		getHandle()->err.printf("Transaction Manager's starting....\n");
	    getHandle()->storeMgr = strMgr;
	    getHandle()->logMgr = logsMgr;
	    return getHandle()->loadConfig();
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

		/* commit transaction in store */
		storeMgr->commitTransaction(tr->getId());

		/* remove transaction from active transactions list */
	    	remove_from_list(tr->getId());
	    	IndexManager::getHandle()->commit(tr->getId()->getId(), id);
		/* free memory */

	    	AllStats::getHandle()->getTransactionsStats()->commitTransaction(tr->getId()->getId());

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

		/* abort transaction in store */
		storeMgr->abortTransaction(tr->getId());

		/* remove transaction from active transactions list */
	    	remove_from_list(tr->getId());

	    	IndexManager::getHandle()->abort(tr->getId()->getId());

	    	AllStats::getHandle()->getTransactionsStats()->abortTransaction(tr->getId()->getId());
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
