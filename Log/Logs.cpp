#include "Logs.h"
#include "LogThread.h"
#include "LogRecord.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


/* LogManager class */

int LogManager::init()
{
  // otwieramy plik z logami
  if( ( fileDes = ::open( LOG_FILE_PATH, O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR ) ) < 0 ) return errno;

  // odpalamy watek
  logThread = new LogThread( fileDes );
  LogRecord::initialize();

  printf( "LogManager: initialize()\n" );

  return 0;
}

int LogManager::destroy()
{
  LogRecord::initialize();
  delete logThread;

  ::close( fileDes );

  printf( "LogManager: destroy()\n" );

  return 0;
}

int LogManager::beginTransaction( TransactionID *tid, unsigned &id )
{
  LogRecord *record = new BeginTransactionRecord( tid );
  logThread->push( record );
  record->getId( id );

  printf( "LogManager: beginTransaction\n");

  return 0;
}

//jeśli tid == NULL to do logów nie jest nic zapisywane
int LogManager::write( TransactionID *tid, LogicalID *lid, string name, DataValue *oldVal, DataValue *newVal, unsigned &id )
{
  LogRecord *record = new WriteRecord( tid, lid, name, oldVal, newVal );
  if(tid != NULL)
    logThread->push( record );
  record->getId( id );

  printf( "LogManager: write\n" );

  return 0;
}

int LogManager::checkpoint( vector<TransactionID *> *tids, unsigned &id )
{
  LogRecord *record = new CkptRecord( tids );
  logThread->push( record );
  record->getId( id );

  printf( "LogManager: checkpoint\n");

  return 0;
}

int LogManager::endCheckpoint( unsigned &id )
{
  LogRecord *record = new EndCkptRecord();
  logThread->push( record );
  record->getId( id );

  printf( "LogManager: end checkpoint\n");

  return 0;
}

int LogManager::commitTransaction( TransactionID *tid, unsigned &id )
{
  //sk
  int err=0;
  
  LogRecord *record = new CommitRecord( tid );
  logThread->push( record );
  record->getId( id );
  
  //sk
  err = logThread->flush();
  
  printf( "LogManager: commitTransaction\n" );

  //sk
  return err;
}

int LogManager::rollbackTransaction( TransactionID *tid, StoreManager *sm, unsigned &id )
{
  LogRecord *record = new RollbackRecord( tid );
  logThread->push( record );
  record->getId( id );

  printf( "LogManager: rollbackTransaction\n" );

  return 0;
}

int LogManager::shutdown( unsigned &id )
{
  LogRecord *record = new ShutdownRecord();
  logThread->push( record );
  logThread->flush();
  record->getId( id );

  printf( "LogManager: shutdown\n" );

  return 0;
}



