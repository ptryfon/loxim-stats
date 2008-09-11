#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

namespace Logs
{
  class LogManager;
}

#include <string>
#include <stdio.h>
#include <vector>
/*#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>*/
#include "../Errors/ErrorConsole.h"
#include "../Store/Store.h"///
#include "LogRecord.h"
#include "../Config/SBQLConfig.h"

using namespace std;
using namespace Errors;
using namespace Store;
using namespace Config;


namespace Logs
{
	struct transaction_info
	{
		int tid;
		/**
		* the LSN of the last log record written by this transaction
		*/
		unsigned int lastLSN;
		/**
		* the LSN of the firstt log record to be undone during a rollback; it's equal to lastLSN unless the latter points to a Compensation Log Record
		*/
		unsigned int undoNxtLSN;
	};
	class hashTransaction_info 
	{
		private:
			hash<int> hasher;

		public:
			size_t operator()(const transaction_info* ti) const 
			{
				return hasher(ti->tid);
			}
	};
	struct eqTransaction_info
	{
		bool operator()(const transaction_info* ti1, const transaction_info* ti2) const{
			return ti1->tid == ti2->tid;
		}
	};
	typedef hash_set<transaction_info*, hashTransaction_info, eqTransaction_info> TransactionTableHash;
	
	class LogTransactionTable
	{
		private:
			TransactionTableHash* ht;
		public:
			LogTransactionTable();
			~LogTransactionTable();
			transaction_info* find(int tid);
			void insert(transaction_info* ti);
			void erase(transaction_info* ti);
	};
	
	class LogManager 
	{
		private:
			ErrorConsole* ec;
			SBQLConfig* config;
			/**
			* Log file descriptor
			*/
			////////int fd;
			/**
			* info about transactions
			*/
			LogTransactionTable* transTable;
			/**
			* LSN of the next log record
			*/
			unsigned int logEndLSN;
		
		public:
int fd; //////////
			LogManager();
			~LogManager();
			
			int init(); ///Server.cpp, XMLIO.cpp, IntegrationTest.cpp
			/// argument storoe jest zbedny
			int start(/*StoreManager *store////*/); ///Server.cpp, XMLIO.cpp, IntegrationTest.cpp
			/// ponizsze do wyrzucenia
			int start(StoreManager *store); ///Server.cpp, XMLIO.cpp, IntegrationTest.cpp
			int shutdown(unsigned &id); /// Server.cpp, XMLIO.cpp, IntegrationTest.cpp
			
			static int checkForBackup(); ///Server.cpp	
			/**
			 * Mowi czy ostatnio serwer zostal czysto zamkniety i czy przed aktualnym uruchomieniem nie bylo potrzebne odtwarzanie bazy
			 */
			static bool isCleanClosed(); ///Server.cpp, IntegrationTest.cpp
			/**
			* Returns the logilac timer value i.e. the next LSN.
			*/
			unsigned int getLogicalTimerValue(); /// Map.cpp, NamedItems.cpp, NamedRoots.cpp, Views.cpp
			/**
			* Forces all the dirty log pages up to the toLSN offset to the disc(for the WAL protocol).
			* To be used by Buffer before writing a page to disc. Sync up to page_header.timestamp.
			*/
			int syncLog(unsigned int toLSN);
			
			int beginTransaction(int tid, unsigned int &id); ///Transaction.cpp
			int commitTransaction(int tid, unsigned int &id); ///Transaction.cpp
			/// argument storoe jest zbedny
			int rollbackTransaction(int tid, /*StoreManager *sm, ////*/unsigned int &id); ///Transaction.cpp
			/// ponizsze do wyrzucenia
			int rollbackTransaction(int tid, StoreManager *sm, unsigned int &id); ///Transaction.cpp
			/**
			* For data updates logging. 
			*/ /// ???hmmm...
			/// argument newLID jest zbedny
			int write(int tid, LogicalID *lid, string name, DataValue *oldVal, DataValue *newVal, unsigned int &LSN/*, bool newLID = false*/); /// DBStoreManager.cpp, 
////			int addRoot(int tid, LogicalID *lid, unsigned &id); /// wykomentowane DBStoreManager.cpp
////			int removeRoot(int tid, LogicalID *lid, unsigned &id); /// wykomentowane DBStoreManager.cpp
			/**
			* Rozpoczyna tworzenie niespoczynkowego punktu kontrolnego.
			*/ /// ??? ale czemu jest uzywane w tak dziwnym miejscu???
			int checkpoint(vector<int> *tids, unsigned int &id); /// DBStoreManager.cpp
			/**
			* Konczy tworzenie bezkonfliktowego punktu kontrolnego.
			*/ /// ???
			int endCheckpoint(unsigned int &id); /// DBStoreManager.cpp
	};
}
#endif // __LOG_MANAGER_H__
