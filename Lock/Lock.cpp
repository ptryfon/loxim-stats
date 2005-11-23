#include <stdio.h>
#include "Lock.h"

namespace LockMgr{


/*_____LockManager___________________________________________________*/
    LockManager* LockManager::lockMgr = new LockManager();

    int LockManager::lock(LogicalID* lid, TransactionID* tid, AccessMode mode)
    {	
        
	return 0;
    }
    int LockManager::unlockAll(int transaction_id)
    {
	set<DBPhysicalID*>* locks = (*transaction_locks)[transaction_id];
	for (set<DBPhysicalID*>::iterator iter = locks->begin();
	     iter != locks->end(); iter++)
	{
	    unlock(*iter);
	}
	return 0;
    }
    int LockManager::unlock(DBPhysicalID* pid)
    {
	return 0;
    }
    LockManager* LockManager::getHandle() { return lockMgr; }
    
    LockManager::LockManager() 
    {
	transaction_locks = new map<int , set<DBPhysicalID*>*>;
	array_of_locks    = new map<DBPhysicalID*, SingleLock*>;
    }


/*_____SingleLock___________________________________________________*/    
    SingleLock::SingleLock(int current, AccessMode mode)
    {
	this->current = current;
	this->current_mode = mode;
	wait_readers = new deque<int>(0);
	wait_writers = new deque<int>(0);
    }
    void SingleLock::addReader(int reader_id)
    {
	wait_readers->push_back(reader_id);
    }
    void SingleLock::addWriter(int writer_id)
    {
	wait_writers->push_back(writer_id);
    }
    int SingleLock::getReader() 
    { 
	int reader = wait_readers->front();
	wait_readers->pop_front();
	return reader;
    }
    int SingleLock::getWriter()
    {
	int writer = wait_writers->front();
	wait_writers->pop_front();
	return writer;
    }
    int SingleLock::is_reader()
    {
	return !wait_readers->empty();
    }
    int SingleLock::is_writer()
    {
	return !wait_writers->empty();
    }
    void SingleLock::setCurrent(int current, AccessMode mode)
    {
	this->current = current;
	this->current_mode = mode;
    }
}
