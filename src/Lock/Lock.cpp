
#include <Lock/Lock.h>

using namespace Util;

/**
 *	@author Dominik Klimczak (dominik.klimczak@students.mimuw.edu.pl)
 *	@author Julian Krzemi�ski (julian.krzeminski@students.mimuw.edu.pl)
 */
namespace LockMgr
{


/*_____LockManager___________________________________________________*/


    /* the only instance of class */
    LockManager* LockManager::lockMgr = NULL;

    LockManager* LockManager::getHandle() { return lockMgr; }

    void LockManager::init() {
    	lockMgr = new LockManager();;
    }

    LockManager::LockManager() : err(ErrorConsole::get_instance(EC_LOCK_MANAGER))
    {
		transaction_locks = new TransactionIdMap;
		map_of_locks      = new DBPhysicalIdMap;
		single_lock_id	  = 0;
		mutex = new Mutex();
    }

    LockManager::~LockManager()
    {
		debug_printf(err, "LockManager - destructor\n");
		delete mutex;

		/*
		 * TransactionManager removes all locks that is why structures:
		 * transaction_locks, map_of_locks should be empty,
		 * hovever, just in case, we erase (lost) elements from these structures
		 */
		for (TransactionIdMap::iterator iter = transaction_locks->begin();
	    	iter != transaction_locks->end(); iter++ )

	    	delete (iter->second);

		delete transaction_locks;
		delete map_of_locks;
    }

    int LockManager::lock(LogicalID* lid, TransactionID* tid, AccessMode mode)
    {
		Mutex::Locker l(*mutex);
		return lock_primitive(lid, tid, mode, l);
    }

    int LockManager::lock_primitive(LogicalID* lid, TransactionID* tid, AccessMode mode, Mutex::Locker &l)
    {
		int errorNumber = 0;

	    	DBPhysicalID* phid = lid->getPhysicalID(tid);
		if (phid == NULL)
			return ENoFile;
	    	DBPhysicalIdMap::iterator pos = map_of_locks->find(phid);

		SingleLock* lock = NULL;

	    	if (pos == map_of_locks->end())
	    	{
			debug_printf(err, "New lock, tid = %d\n", tid->getId() );
			/* creating new single lock */
			RWUJSemaphore* rwsem = new RWUJSemaphore();
			lock = new SingleLock(tid, mode, rwsem, single_lock_id);
			lock->setPHID(phid);

			single_lock_id++;
			(*map_of_locks)[phid] = lock;
	    	}
	    	else
	    	{
			debug_printf(err, "Wait for lock, tid = %d\n", tid->getId() );
			/* modifying existing single lock for this PhysicalID */
			{
				Mutex::Unlocker ul(l);
				lock = pos->second;
				errorNumber = lock->wait_for_lock(tid, mode);

				if (errorNumber) return errorNumber;

			}
		}
		debug_printf(err, "Lock received\n");
		if ((*transaction_locks)[tid] == 0 )
		    (*transaction_locks)[tid] = new SingleLockSet;

		((*transaction_locks)[tid])->insert(lock);


		return errorNumber;
    }

    int LockManager::lockAll(set<LogicalID*>* lock_set, TransactionID* tid, AccessMode mode)
    {
		int errorCode = 0;

		debug_printf(err, "LockAll, tid = %d\n", tid->getId());
		Mutex::Locker l(*mutex);

		set<LogicalID*>::iterator pos = lock_set->begin();
		while(!errorCode && pos != lock_set->end())
		{
		    errorCode = lock_primitive(*pos, tid, mode, l);
		}

		return errorCode;
    }


    int LockManager::unlockAll(TransactionID* transaction_id)
    {
		debug_printf(err, "UnlockAll, tid = %d\n", transaction_id->getId());
		Mutex::Locker l(*mutex);
		TransactionIdMap::iterator pos = transaction_locks->find(transaction_id);
		if (pos != transaction_locks->end())
		{
		    SingleLockSet* locks = pos->second;
		    for (SingleLockSet::iterator iter = locks->begin();
		        iter != locks->end(); iter++)
		    {
				debug_printf(err, "Unlock single, tid = %d\n", transaction_id->getId());
				int delete_lock = unlock((*iter), transaction_id);

				if (delete_lock)
				{
				    DBPhysicalID* phid = (*iter)->getPHID();
				    map_of_locks->erase( phid );

				    delete (*iter);
				}
		    }
		    transaction_locks->erase(transaction_id);
		    delete locks;
		}

		return 0;
    }

    int LockManager::modifyLockLID(DBPhysicalID* oldPHID, LogicalID* newLID, TransactionID* tid)
    {
	int errorNumber = 0;

	debug_printf(err, "modifyLockLID, tid = %d\n", tid->getId());

	{
		Mutex::Locker l(*mutex);
	    DBPhysicalID* newPHID = newLID->getPhysicalID(tid);
	    DBPhysicalIdMap::iterator pos = map_of_locks->find(oldPHID);
	    if (pos != map_of_locks->end())
	    {
		    SingleLock* lock = pos->second;
		    map_of_locks->erase(oldPHID);
		    lock->setPHID(newPHID);
		    (*map_of_locks)[newPHID] = lock;
	    }
	    else errorNumber = -1;
	}

	return errorNumber;
    }

    int LockManager::unlock(SingleLock* lock, TransactionID* tid)
    {
		return lock->unlock(tid);
    }
}
