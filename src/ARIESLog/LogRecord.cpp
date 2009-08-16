#include <Errors/Errors.h>
#include <ARIESLog/LogRecord.h>
#include <iostream>
#include <sstream>


namespace Logs 
{
/* ------------------------- LogRecord ----------------------- */
	unsigned LogRecord::nextLSN = 0;

	LogRecord::~LogRecord()
	{
	}
	
	void LogRecord::init(int nextLSN)
	{
		LogRecord::nextLSN = nextLSN;
	}
	
	Serialized LogRecord::serialize()
	{
		Serialized s2;
		s2 += static_cast<int>(getType());
		s2 += tid;
		s2 += prevLSN;
		s2 += serializeSpecific();
		
		Serialized s;
		s += s2.size;
		s += s2;

		return s;
	}
	int LogRecord::deserialize(unsigned char *buffer)
	{
		int read = 0;
		this->tid = *(reinterpret_cast<int*>(buffer));
		read += sizeof(int);
		this->prevLSN = *(reinterpret_cast<int*>(buffer + read));
		read += sizeof(int);
		read += deserializeSpecific(buffer + read);
		
		return read;
	}
	int LogRecord::read(int fd, unsigned int LSN, LogRecord*& record)
	{
		int length = 1;
		int type;
		if (::lseek(fd, LSN, SEEK_SET) < 0)
			return EBadLSN | ErrLogs;
		::read(fd, &length, sizeof(int));
		unsigned char buffer[length+1]/*, *toRead*/;
		::read(fd, buffer, length);
		buffer[length] = '\0';
		type = *(reinterpret_cast<int*>(buffer));
		////toRead = buffer + sizeof(int);
		
		switch (type)
		{
			case COMPENSATION_RECORD: 
			{
				record = new CompensationLogRecord(LSN);
				break;
			}
			case UPDATE_RECORD: 
			{
				record = new UpdateLogRecord(LSN);
				break;
			}
			case BEGIN_RECORD: 
			{
				record = new BeginLogRecord(LSN);
				break;
			}
			case END_RECORD: 
			{
				record = new EndLogRecord(LSN);
				break;
			}
			/* ...////*/
			default:
				record = NULL;
		};
		record->deserialize(buffer + sizeof(int));
		
		return 0;
	}
	int LogRecord::write(int fd)
	{
		Serialized s = serialize();
		this->LSN = ::lseek(fd, 0, SEEK_END);
		
		int res = ::write(fd, s.bytes, s.size);
		LogRecord::nextLSN = ::lseek(fd, 0, SEEK_END);
		
		return res;
	}
	unsigned int LogRecord::getLSN()
	{
		return LSN;
	}
	unsigned int LogRecord::getUndoNxtLSN()
	{
		return prevLSN;
	}
	bool LogRecord::isCLR()
	{
		return false;
	} 
	/*void LogRecord::setPrevLSN(unsigned int prevLSN)
	{
		this->prevLSN = prevLSN;
	}*/ /// zbedne?
	undo_data* LogRecord::getUndoData()
	{
		return NULL;
	}
	LogicalID* LogRecord::getLid()
	{
		return NULL;
	}

/* ------------------------- CompensationLogRecord ----------------------- */
	
	CompensationLogRecord::CompensationLogRecord()
	{
	}
	CompensationLogRecord::CompensationLogRecord(unsigned int LSN)
	{
		this->LSN = LSN;
	}
	CompensationLogRecord::CompensationLogRecord(unsigned int prevLSN, int /*tid*/, LogicalID* lid, unsigned int undoNxtLSN)
	{
		this->prevLSN = prevLSN;
		this->lid = lid;
		this->undoNxtLSN = undoNxtLSN;
	}
	CompensationLogRecord::~CompensationLogRecord()
	{
		delete this->lid;
	}
	Serialized CompensationLogRecord::serializeSpecific()
	{
		Serialized s;
		s += lid->serialize();
		s += undoNxtLSN;

		return s;
	}
	int CompensationLogRecord::deserializeSpecific(unsigned char *buffer)
	{
		///int read = DBLogicalID::deserialize(buffer + 0, reinterpret_cast<DBLogicalID*&>(this->lid));
		int read = StoreManager::theStore->logicalIDFromByteArray(buffer, lid);
		this->undoNxtLSN = *(reinterpret_cast<unsigned int*>(buffer + read));
		
		return read + sizeof(unsigned int);
	}
	int CompensationLogRecord::undo()
	{
		return 0;
	}
	int CompensationLogRecord::redo()
	{
		return 0;
	}
	LogRecordType CompensationLogRecord::getType()
	{
		return COMPENSATION_RECORD;
	}
	bool CompensationLogRecord::isCLR()
	{
		return true;
	}
	unsigned int CompensationLogRecord::getUndoNxtLSN()
	{
		return undoNxtLSN;
	}
	LogicalID* CompensationLogRecord::getLid()
	{
		return lid;
	}
	/*void CompensationLogRecord::setUndoNxtLSN(unsigned int undoNxtLSN)
	{
		this->undoNxtLSN = undoNxtLSN;
	}*/ /// zbedne?

/* ------------------------- UpdateLogRecord ----------------------- */
	
	UpdateLogRecord::UpdateLogRecord()
	{
	}
	UpdateLogRecord::UpdateLogRecord(unsigned int LSN)
	{
		this->LSN = LSN;
	}
	UpdateLogRecord::UpdateLogRecord(unsigned int prevLSN, int tid, LogicalID* lid, string name, DataValue *oldVal, DataValue *newVal)
	{
		this->prevLSN = prevLSN;
		this->lid = lid;
		this->tid = tid;
		this->undoData = new undo_data();
		this->undoData->name = name;
		this->undoData->oldVal = oldVal;
		this->undoData->newVal = newVal;
	}
	UpdateLogRecord::~UpdateLogRecord()
	{
		delete this->lid;
		delete this->undoData->oldVal;
		delete this->undoData->newVal;
		delete this->undoData;
	}
	LogRecordType UpdateLogRecord::getType()
	{
		return UPDATE_RECORD;
	}
	Serialized UpdateLogRecord::serializeSpecific()
	{
		Serialized s;
		s += lid->serialize();
		s += static_cast<int>(undoData->oldVal != NULL);
		if (undoData->oldVal != NULL) 
			s += undoData->oldVal->serialize();
		s += static_cast<int>(undoData->newVal != NULL);
		if (undoData->newVal != NULL)
			s += undoData->newVal->serialize();
		s += undoData->name;
		return s;
	}
	int UpdateLogRecord::deserializeSpecific(unsigned char *buffer)
	{
		///int read = DBLogicalID::deserialize(buffer, reinterpret_cast<DBLogicalID*&>(this->lid));
		int read = StoreManager::theStore->logicalIDFromByteArray(buffer, lid);
		this->undoData = new undo_data();

		if ( *(reinterpret_cast<int*>(buffer + read)))
			///read += DBDataValue::deserialize(/*this->tid////*/NULL, buffer + read + sizeof(int), reinterpret_cast<DBDataValue*&>(this->undoData->oldVal), /*true///*/false); //// dlaczego true???
			read += StoreManager::theStore->dataValueFromByteArray(/*this->tid////*/NULL, buffer + read + sizeof(int), this->undoData->oldVal);
		else
			this->undoData->oldVal = NULL;
		read +=  sizeof(int);

		if ( *(reinterpret_cast<int*>(buffer + read)))
			///read += DBDataValue::deserialize(/*this->tid////*/NULL, buffer + read + sizeof(int), reinterpret_cast<DBDataValue*&>(this->undoData->newVal), /*true///*/false); //// dlaczego true???
			read += StoreManager::theStore->dataValueFromByteArray(/*this->tid////*/NULL, buffer + read + sizeof(int), this->undoData->newVal);
		else
			this->undoData->newVal = NULL;
		read +=  sizeof(int);

		int namelen = *(reinterpret_cast<int*>(buffer + read));
		read +=  sizeof(int);
		this->undoData->name = "";
		for (int i=0; i<namelen; i++) 
			this->undoData->name += *(reinterpret_cast<char*>(buffer + read + i));
		///this->undoData->name = (reinterpret_cast<char*>(buffer + read));
		
		return read + undoData->name.length();
	}
	int UpdateLogRecord::undo()
	{
cout << "undo!\n"; ////
		return 0;
	}
	undo_data* UpdateLogRecord::getUndoData()
	{
		return undoData;
	}
	LogicalID* UpdateLogRecord::getLid()
	{
		return lid;
	}
	int UpdateLogRecord::redo()
	{
		return 0;
	}
	
	/* ------------------------- BeginLogRecord ----------------------- */
	
	BeginLogRecord::BeginLogRecord()
	{
	}
	BeginLogRecord::BeginLogRecord(unsigned int LSN)
	{
		this->LSN = LSN;
	}
	BeginLogRecord::BeginLogRecord(unsigned int /*prevLSN*/, int tid)
	{
		this->prevLSN = 0;
		this->tid = tid;
	}
	BeginLogRecord::~BeginLogRecord()
	{
	}
	LogRecordType BeginLogRecord::getType()
	{
		return BEGIN_RECORD;
	}
	Serialized BeginLogRecord::serializeSpecific()
	{
		Serialized s;
		return s;
	}
	int BeginLogRecord::deserializeSpecific(unsigned char */*buffer*/)
	{
		return 0;
	}
	int BeginLogRecord::undo()
	{
		return 0;
	}
	int BeginLogRecord::redo()
	{
		return 0;
	}
	
	/* ------------------------- EndLogRecord ----------------------- */
	
	EndLogRecord::EndLogRecord()
	{
	}
	EndLogRecord::EndLogRecord(unsigned int LSN)
	{
		this->LSN = LSN;
	}
	EndLogRecord::EndLogRecord(unsigned int prevLSN, int tid)
	{
		this->prevLSN = prevLSN;
		this->tid = tid;
	}
	EndLogRecord::~EndLogRecord()
	{
	}
	LogRecordType EndLogRecord::getType()
	{
		return END_RECORD;
	}
	Serialized EndLogRecord::serializeSpecific()
	{
		Serialized s;
		return s;
	}
	int EndLogRecord::deserializeSpecific(unsigned char */*buffer*/)
	{
		return 0;
	}
	int EndLogRecord::undo()
	{
		return 0;
	}
	int EndLogRecord::redo()
	{
		return 0;
	}
} 
