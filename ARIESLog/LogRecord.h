#ifndef __LOG_RECORD_H__
#define __LOG_RECORD_H__

namespace Logs
{
	class LogRecord;
	class CompensationLogRecord;
	class UpdateLogRecord;
	class BeginLogRecord;
}

#include <string>
#include <stdio.h>
#include <set>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "../Errors/ErrorConsole.h"
///#include "tmp.h" ///
#include "../Store/Store.h"
#include "../Config/SBQLConfig.h"
#include "../QueryExecutor/HashMacro.h"

#define DEBUG_MODE ///
#define LOGS_TEST ///


using namespace Errors;
using namespace Store;
using namespace Config;

namespace Store /// tmp
{
	typedef struct Serialized Serialized;	
}

namespace Logs
{	
	enum LogRecordType {COMPENSATION_RECORD=1, UPDATE_RECORD=2, 
		BEGIN_RECORD=3, END_RECORD=4, COMMIT_RECORD=5, ROLLBACK_RECORD=6}; /// to jeszcze nie wszystkie ?

	typedef struct undo_data {
		/**
		* if oldVal == NULL an object had been created (DBStoreManager::createObject()), and if newVal == NULL an object had been deleted (DBStoreManager::deleteObject())
		*/
		DataValue *oldVal;
		DataValue *newVal;
		/**
		* irrelevant for object deletion
		*/
		string name;
	} undo_data; /// jeszcze nie wszystko?
	
	class LogRecord 
	{
		protected:
			static unsigned int nextLSN;
			/**
			*  Log Sequential Number, here the offset in the log file
			*/
			unsigned int LSN;
			////LogRecordType type;
			/**
			* transaction ID
			*/
			int tid;
			/**
			* LSN of the transaction's preceding log record
			*/
			unsigned int prevLSN;
			///undo_data* undoData;
			/// redo_data ????
			
			virtual LogRecordType getType() = 0;
			
			virtual Serialized serialize();
			virtual Serialized serializeSpecific() = 0;
			virtual int deserialize(unsigned char *buffer);
			virtual int deserializeSpecific(unsigned char *buffer) = 0;
			
		public:
			static unsigned int getIdSeq() { return LogRecord::nextLSN; }
			static void init(int nextLSN);
			/**
			* reads a log record from a file; LSN is the record's offset
			*/
			static int read(int fd, unsigned int LSN, LogRecord*& record);
			/**
			* writes a log record to a file and sets the record's LSN
			*/
			virtual int write(int fd);
			virtual int undo() = 0;
			virtual undo_data* getUndoData();
			virtual int redo() = 0;
			
			unsigned int getLSN();
			virtual unsigned int getUndoNxtLSN();
			virtual LogicalID* getLid();
			virtual bool isCLR();
			/*void setPrevLSN(unsigned int prevLSN);*/ /// zbedne?
	};
	
	class CompensationLogRecord : public LogRecord 
	{
		protected:
			LogicalID* lid; //DBPhysicalID* pid; /// page/object id ???? // only Compensation and UpdateLog Records
			/**
			* only Compensation Log Records; next record to be undone == prevLSN of the record compensated by this record
			*/
			unsigned int undoNxtLSN;
			/// redo_data ????
			
			LogRecordType getType();
			
			Serialized serializeSpecific();
			int deserializeSpecific(unsigned char *buffer);
			
		public:
			CompensationLogRecord();
			CompensationLogRecord(unsigned int LSN);
			CompensationLogRecord(unsigned int prevLSN, int tid, LogicalID* lid, unsigned int undoNxtLSN);
			~CompensationLogRecord();
			
			int undo();
			int redo();
			
			unsigned int getUndoNxtLSN();
			LogicalID* getLid();
			bool isCLR();
			/*void setUndoNxtLSN(unsigned int undoNxtLSN);*/ /// zbedne?
	};
	
	class UpdateLogRecord : public LogRecord 
	{
		protected:
			LogicalID* lid; //DBPhysicalID* pid; /// page/object id ???? // only Compensation and UpdateLog Records
			/**
			* directions for undoing the operation or NULL if the log record is redo-only
			*/
			undo_data* undoData;
			/// redo_data ????
			
			LogRecordType getType();
			
			Serialized serializeSpecific();
			int deserializeSpecific(unsigned char *buffer);
			
		public:
			UpdateLogRecord();
			UpdateLogRecord(unsigned int LSN);
			UpdateLogRecord(unsigned int prevLSN, int tid, LogicalID* lid, string name, DataValue *oldVal, DataValue *newVal);
			~UpdateLogRecord();
			
			int undo();
			undo_data* getUndoData();
			LogicalID* getLid();
			int redo();
	};
	
	class BeginLogRecord : public LogRecord 
	{
		protected:			
			LogRecordType getType();
			
			Serialized serializeSpecific();
			int deserializeSpecific(unsigned char *buffer);
			
		public:
			BeginLogRecord();
			BeginLogRecord(unsigned int LSN);
			BeginLogRecord(unsigned int prevLSN, int tid); /// jesli int tid zmienie na TransactionID tid, to ten pierwszy argument mozna wyrzucic
			~BeginLogRecord();
			
			int undo();
			int redo();
	};
	
	class EndLogRecord : public LogRecord 
	{
		protected:			
			LogRecordType getType();
			
			Serialized serializeSpecific();
			int deserializeSpecific(unsigned char *buffer);
			
		public:
			EndLogRecord();
			EndLogRecord(unsigned int LSN);
			EndLogRecord(unsigned int prevLSN, int tid); /// jesli int tid zmienie na TransactionID tid, to ten pierwszy argument mozna wyrzucic
			~EndLogRecord();
			
			int undo();
			int redo();
	};
}
#endif
