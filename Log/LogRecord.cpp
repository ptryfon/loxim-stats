#include "LogRecord.h"
#include "LogIO.h"
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


/* LogRecord class */

int LogRecord::initialize()
{
  idSeq = 0;
  new BeginTransactionRecord();
  new WriteRecord();
  new CkptRecord();
  new EndCkptRecord();
  new CommitRecord();
  new RollbackRecord();

  return 0;
}

int LogRecord::destroy()
{
  for( map< int, LogRecord* >::iterator it = dictionary.begin(); it != dictionary.end(); it++ )
  {
    delete (*it).second;
  }

  return 0;
}

int LogRecord::readLogRecordForward( LogRecord *&result, int fileDes )
{
  int recordType;
  int errCode;
  off_t filePosBegin, filePosEnd;
  int recordLen;

  if( ( errCode = LogIO::getFilePos( fileDes, filePosBegin ) ) ) return errCode;
  if( ( errCode = LogIO::readInt( fileDes, recordType ) ) ) return errCode;
  if( !dictionary.count( recordType ) ) return UNKNOWN_LOG_RECORD_TYPE_ERROR;

  if( ( errCode = dictionary[recordType]->instance( result ) ) ) return errCode;
  result->read( fileDes );

  if( ( errCode = LogIO::readInt( fileDes, recordLen ) ) ) return errCode;
  if( ( errCode = LogIO::getFilePos( fileDes, filePosEnd ) ) ) return errCode;

  if( filePosEnd - filePosBegin != recordLen ) return LOG_RECORD_LENGTH_MISMATCH_ERROR;

  return 0;
}

int LogRecord::readLogRecordBackward( LogRecord *&result, int fileDes )
{
  int errCode;
  int recordLen;
  off_t filePos;

  if( ( errCode = LogIO::getFilePos( fileDes, filePos ) ) ) return errCode;
  if( !filePos )
  {
    result = NULL;
    return 0;
  }

  // cofamy sie o rozmiar int'a do tylu
  if( ::lseek( fileDes, - sizeof( int ), SEEK_CUR ) < 0 ) return errno;
  if( ( errCode = LogIO::readInt( fileDes, recordLen ) ) ) return errCode;

  // cofamy sie o rozmiar rekordu do tylu
  if( ::lseek( fileDes, - recordLen, SEEK_CUR ) < 0 ) return errno;

  if( ( errCode = readLogRecordForward( result, fileDes ) ) ) return errCode;

  // ustawiamy sie przed odczytanym wlasnie rekordem
  if( ::lseek( fileDes, - recordLen, SEEK_CUR ) < 0 ) return errno;

  return 0;
}

int LogRecord::writeLogRecord( LogRecord *recordPtr, int fileDes )
{
  int errCode;
  off_t filePosBegin, filePosEnd;
  int recordType;

  if( ( errCode = recordPtr->getType( recordType ) ) ) return errCode;
  if( ( errCode = LogIO::getFilePos( fileDes, filePosBegin ) ) ) return errCode;

  // zapisujemy typ rekordu
  if( ( errCode = LogIO::writeInt( fileDes, recordType ) ) ) return errCode;

  // zapisujemy rekord
  if( ( errCode = recordPtr->write( fileDes ) ) ) return errCode;

  // zapisujemy dlugosc rekordu
  if( ( errCode = LogIO::getFilePos( fileDes, filePosEnd ) ) ) return errCode;
  if( ( errCode = LogIO::writeInt( fileDes, filePosEnd - filePosBegin + sizeof( int ) ) ) ) return errCode;

  return 0;
}


/* TransactionRecord class */

int TransactionRecord::read( int fileDes ) { return LogIO::readTransactionID( tid, fileDes ); }

int TransactionRecord::write( int fileDes ) { return LogIO::writeTransactionID( tid, fileDes ); }



/* CkptRecord class */

int CkptRecord::read( int fileDes ) { return LogIO::readTransactionIDVector( tidVec, fileDes ); }
int CkptRecord::write( int fileDes ) { return LogIO::writeTransactionIDVector( tidVec, fileDes ); }


/* WriteRecord class */

int WriteRecord::read( int fileDes )
{
  int errCode;

  if( ( errCode = TransactionRecord::read( fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::readTransactionID( tid, fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::readLogicalID( lid, fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::readDataValue( oldVal, fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::readDataValue( newVal, fileDes ) ) ) return errCode;

  return errCode;
}

int WriteRecord::write( int fileDes )
{
  int errCode;

  if( ( errCode = TransactionRecord::write( fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::writeLogicalID( lid, fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::writeDataValue( oldVal, fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::writeDataValue( newVal, fileDes ) ) ) return errCode;

  return errCode;
}