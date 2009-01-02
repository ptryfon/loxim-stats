#include <ARIESLog/Logs.h>
#include <Errors/Errors.h>
#include <SystemStats/AllStats.h>
#include <string.h>

namespace Logs 
{
/* ------------------------- LogTransactionTable ----------------------- */
	LogTransactionTable::LogTransactionTable()
	{
		this->ht = new TransactionTableHash();
	}
	LogTransactionTable::~LogTransactionTable()
	{
		vector<transaction_info*> tisToDel;
		for(TransactionTableHash::iterator i = ht->begin(); i != ht->end(); i++) {
			tisToDel.push_back(*i);
		}
		delete ht;
		for(vector<transaction_info*>::iterator i = tisToDel.begin(); i != tisToDel.end(); ++i) {
			delete (*i);
		}
	}
	transaction_info* LogTransactionTable::find(int tid)
	{
		transaction_info ti;
		ti.tid = tid;
		TransactionTableHash::iterator it = ht->find(&ti);
		if (it == ht->end())
			return NULL;
		else
			return (*it);
	}
	void LogTransactionTable::insert(transaction_info* ti)
	{
		ht->insert(ti);
	}
	void LogTransactionTable::erase(transaction_info* ti)
	{
		ht->erase(ti);
	}

/* ------------------------- LogManager ----------------------- */	
	LogManager::LogManager() 
	{
		ec = &ErrorConsole::get_instance(EC_ARIES_LOG);
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::Constructor");
#endif
		fd = -1;
	}
	
	LogManager::~LogManager() 
	{
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::Destructor beginning");
#endif
		if(fd != -1) 
		{
			if(::close(fd) < 0)
				debug_printf(*ec, "close: errno = %d, errmsg = %s",
					errno, strerror(errno));
			fd = -1;
		}
		if(config) 
		{
			delete config;
			config = NULL;
		}
		if (transTable)
		{
			delete transTable;
			transTable = NULL;
		}
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::Destructor done");
#endif
	}
	
	int LogManager::init() 
	{
		string logFilePath;
		
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::init beginning");
#endif
		config = new SBQLConfig("ARIESLog");
		transTable = new LogTransactionTable();
		if (config->getString("ARIESLog_file", logFilePath) != 0)
			logFilePath = "/tmp/ARIESLog";
		if ((fd = ::open(logFilePath.c_str(), 
			O_RDWR | O_CREAT, S_IWUSR | S_IRUSR)) < 0)
				return EBadFile | ErrLogs;
		LogRecord::init(::lseek(fd, 0, SEEK_END));
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::init done");
#endif
		return 0;
	}
	
	int LogManager::start(/*StoreManager *store = NULL///*/) 
	/// argument store jest zbedny
	{
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::start");
#endif
		
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::start done");
#endif
		return 0;
	}
	int LogManager::start(StoreManager *store = NULL) 
	/// argument store jest zbedny
	{
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::start");
#endif
		
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::start done");
#endif
		return 0;
	}
	
	int LogManager::shutdown(unsigned &id) 
	{
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::shutdown");
#endif
		
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager::shutdown done");
#endif
		return 0;
	};
	
	int LogManager::checkForBackup()
	{
		return 0;
	}
	
	bool LogManager::isCleanClosed()
	{
		return true;
	}
	
	unsigned int LogManager::getLogicalTimerValue()
	{
		//return logEndLSN;
		//return ::lseek(fd, 0, SEEK_END); ////
		return LogRecord::getIdSeq();////
	}
	
	int LogManager::syncLog(unsigned int toLSN)
	{
		if (sync_file_range(fd, 0, toLSN, SYNC_FILE_RANGE_WAIT_BEFORE | SYNC_FILE_RANGE_WRITE | SYNC_FILE_RANGE_WAIT_AFTER) < 0)
			return ESyncLog;
#ifdef DEBUG_MODE
		debug_printf(*ec, "LogManager::syncLog up to %d done.\n", toLSN);
#endif
		return 0;
	}
	
	
	int LogManager::beginTransaction(int tid, unsigned int &id)
	{
		BeginLogRecord* record = new BeginLogRecord(0, tid);
		int res = record->write(fd);
		
		transaction_info* ti = new transaction_info();
		ti->tid = tid;
		ti->lastLSN = record->getLSN();
		delete record;
		ti->undoNxtLSN = ti->lastLSN;
		transTable->insert(ti);
		///czy to juz wszystko????

		id = ti->lastLSN;
#ifdef DEBUG_MODE
		debug_printf(*ec, "LogManager::transaction %d started with LSN = %d\n", tid, id);
#endif
		return 0;
	}
	
	int LogManager::commitTransaction(int tid, unsigned int &id)
	{
		transaction_info* ti = transTable->find(tid);
		int res = endTransaction(ti, id);
#ifdef DEBUG_MODE
		debug_printf(*ec, "LogManager::transaction %d commited with LSN = %d\n", tid, id);
#endif
		return res;
	}
	
	int LogManager::endTransaction(transaction_info* ti, unsigned int &id)
	{
		if (ti != NULL) /// tmp
		{
			EndLogRecord* record = new EndLogRecord(ti->lastLSN, ti->tid);
			int res = record->write(fd);		
			id = record->getLSN();
			delete record;
			
			/**
			* Making sure the changes made by the transaction will not be lost.
			*/
			syncLog(id);
			transTable->erase(ti);
			///czy to juz wszystko????
		
			delete ti;
		};
		return 0;
	}
	
	int LogManager::rollbackTransaction(int tid, /*StoreManager *sm, ////*/unsigned int &id)
	{
#ifdef DEBUG_MODE
		debug_printf(*ec, "LogManager::rollbackTransaction %d\n", tid);
#endif
		transaction_info* ti = transTable->find(tid);
		/*RollbackLogRecord* record = new RollbackLogRecord(-1, tid);
		
		int res = record->write(fd);
		ti->lastLSN = record->getLSN();
		delete record;
		ti->undoNxtLSN = ti->lastLSN;
		///czy to juz wszystko????
		*/
		unsigned int undoNxtLSN = ti->undoNxtLSN;
		LogRecord* record;
		undo_data* undoData;
		while (undoNxtLSN > 0)
		{
#ifdef DEBUG_MODE
			debug_printf(*ec, "LogManager:: LSN of the record to be undone: %d\n", undoNxtLSN);
#endif
			LogRecord::read(fd, undoNxtLSN, record);
			undoNxtLSN = record->getUndoNxtLSN();
			record->undo();/////////!!!
			undoData = record->getUndoData();
			if (undoData)
			{
cout << "undoData name: '" << undoData->name << "'\n";////////////!!!
				if (undoData->oldVal != NULL)
				{
cout << "undoData oldVal: '" << undoData->name << "'\n";////////////!!!
					if (undoData->newVal == NULL)
					{
cout << "undoing delete object\n";////////////!!!
#ifndef LOGS_TEST
						/// !!! Ma byc cos w stylu undeleteObject
						ObjectPointer* object;
						LogicalID* lid = record->getLid();
						lid = (lid ? lid->clone() : NULL);
						/// ponizsze to tylko przymiarka - tak nie moze zostac
						StoreManager::theStore->createObject(NULL /*////*/, undoData->name, undoData->oldVal->clone(), object, lid);	
#endif
						}
					else
					{
cout << "undoData newVal: '" << undoData->newVal->toString() << "'\n";////////////!!!
cout << "undoing modify object (hypothetical)\n";////////////!!!
					}
				}
				else
					if (undoData->newVal != NULL)
					{
cout << "undoing create object\n";////////////!!!
cout << "undoData newVal: '" << undoData->newVal->toString() << "'\n";////////////!!!
#ifndef LOGS_TEST
						/// !!! Ma byc cos w stylu uncreateObject
						LogicalID* lid = record->getLid();
						lid = (lid ? lid->clone() : NULL);
						ObjectPointer* object = StoreManager::theStore->createObjectPointer(lid, undoData->name, undoData->newVal->clone());
						/// ponizsze to tylko przymiarka - tak nie moze zostac
						StoreManager::theStore->deleteObject(NULL /*////*/, object);
#endif
						};
			}
			delete record;
		};
		
		endTransaction(ti, id);
#ifdef DEBUG_MODE
		debug_print(*ec,  "LogManager: rollback finished.\n");
#endif
		return 0;
	}
	int LogManager::rollbackTransaction(int tid, StoreManager *sm, unsigned int &id)
	{
		return rollbackTransaction(tid, id);
	}
	
	int LogManager::write(int tid, LogicalID *lid, string name, DataValue *oldVal, DataValue *newVal, unsigned int &id/*, bool newLID = false*/)
	{
		int res = 0;
		transaction_info* ti = transTable->find(tid);
		if (ti != NULL) /// tmp
		{
			UpdateLogRecord* record = new UpdateLogRecord(ti->lastLSN, tid, lid, name, oldVal, newVal);
			
			res = record->write(fd);
			ti->lastLSN = record->getLSN();
			delete record;
			ti->undoNxtLSN = ti->lastLSN;
			///czy to juz wszystko????
			
			id = ti->lastLSN;
#ifdef DEBUG_MODE
			debug_printf(*ec, "LogManager::transaction %d: update written to the log with LSN = %d\n", tid, id);
#endif
		};
		
		return res;
	}
	
	int LogManager::checkpoint(vector<int> *tids, unsigned int &id)
	{
		return 0;
	}
	
	int LogManager::endCheckpoint(unsigned int &id)
	{
		return 0;
	}
} 
