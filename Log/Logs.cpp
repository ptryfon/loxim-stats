#include "Logs.h"
#include "LogThread.h"
#include "LogRecord.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


/* LogManager class */

void LogManager::pushLogable( int tid, LogRecord *record)
{
  if(tid >= 0)
    logThread->push( record );
}

int LogManager::flushLog()
{
  if(logThread == NULL)
    return LOG_UNINITIALIZED_LOG_THREAD;
  return logThread->flush();
}

int LogManager::init()
{
  int errCode = 0;

  config = new SBQLConfig( "Log" );
  //config->init( "../Server/Server.conf" );
  if( ( errCode = config->getString( "logspath", logFilePath ) ) ) return errCode;

  printf( "LogsPath: %s\n", logFilePath.c_str() );

  // nie wykonujecie tej metody!
  // jedyne miejsce gdzie ec->init ma prawo wystapic to main()

  // no kurde! A gdzie jest wolane LogManager::init()? Nie w MAIN()?!
  // i skad ja w main() wezme ec, jesli to jest chronione pole LogManagera?
  ec->init(2); // baggins, prosze - nie kasuj!

  // otwieramy plik z logami
  if( ( fileDes = ::open( logFilePath.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR ) ) < 0 ) return errno;

  ::lseek( fileDes, 0, SEEK_END );
  // odpalamy watek
  logThread = new LogThread( fileDes );
  LogRecord::initialize();

  *ec << "LogManager: initialize()";

  return 0;
}

int LogManager::start( StoreManager *store )
{
  CrashRecovery *cr = new CrashRecovery();
  int errCode = 0;

  errCode = cr->init( this, store );
  if( errCode == 0 )
    errCode = cr->Recover();

  delete cr;

  return errCode;
}

int LogManager::destroy()
{
  LogRecord::initialize();
  delete logThread;

  ::close( fileDes );

  printf( "LogManager: destroy()\n" );

  return 0;
}

int LogManager::beginTransaction( int tid, unsigned &id )
{
  LogRecord *record = new BeginTransactionRecord( tid );
  logThread->push( record );
  record->getId( id );

  printf( "LogManager: beginTransaction\n" );

  return 0;
}

//jeśli tid == NULL to do logów nie jest nic zapisywane
int LogManager::write( int tid, LogicalID *lid, string name, DataValue *oldVal, DataValue *newVal, unsigned &id )
{
  LogRecord *record = new WriteRecord( tid, lid, name, oldVal, newVal );
  pushLogable( tid, record );
  record->getId( id );

  printf( "LogManager: write\n" );

  return 0;
}

int LogManager::addRoot( int tid, LogicalID *lid, unsigned &id )
{
  LogRecord *record = new AddRootRecord( tid, lid);
  pushLogable( tid, record );
  record->getId( id );

  printf( "LogManager: addRoot\n");
  return 0;
}

int LogManager::removeRoot( int tid, LogicalID *lid, unsigned &id )
{
  LogRecord *record = new RemoveRootRecord( tid, lid);
  pushLogable( tid, record );
  record->getId( id );

  printf( "LogManager: addRoot\n" );
  return 0;
}

int LogManager::checkpoint( vector<int> *tids, unsigned &id )
{
  LogRecord *record = new CkptRecord( tids );
  logThread->push( record );
  record->getId( id );

  printf( "LogManager: checkpoint\n" );

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

int LogManager::commitTransaction( int tid, unsigned &id )
{
  //sk
  int err=0;
  
  LogRecord *record = new CommitRecord( tid );
  logThread->push( record );
  record->getId( id );
  
  //sk
  err = flushLog();
  
  printf( "LogManager: commitTransaction\n" );

  //sk
  return err;
}

int LogManager::printRollbackSign(int tid)
{
  LogRecord *record = new RollbackRecord( tid );
  logThread->push( record );
  return 0;
}

int LogManager::rollbackTransaction( int tid, StoreManager *sm, unsigned &id )
{
  int fd;
  int err=0;
  SetOfTransactionIDS setOfTIDs;
  if( ( err = flushLog() ) > 0 )
    return err;
  if( ( fd = ::open( logFilePath.c_str(), O_RDONLY) ) < 0 ) return errno;
  //przesunięcie na koniec bo czytamy od końca.
  //UWAGA! jeśli może być tak, że  rozmiar pliku zmienia się o część zapisanych w pliku danych, to będzie źle
  //ale mam wrażenie, że tak niejest.
  if( ::lseek(fd,0,SEEK_END) < 0 ) return errno;
  setOfTIDs.insert(tid);

  //pętla jest wykonywana dopóki nie znajdzie się begin który usunie tranzakcję ze zbioru
  while(!setOfTIDs.empty())
  {
    LogRecord* tmp = 0;
    //wczytywanie rekordu dziennika
    if( (err = LogRecord::readLogRecordBackward(tmp, fd, sm) ) > 0)
    {
      delete tmp;
      return err;
    }
    //wykonywanie rollbacka na rekordzie
    if( (err = tmp->rollBack(&setOfTIDs, sm)) >0 )
    {
      delete tmp;
      return err;
    }
    delete tmp;
  }
  close(fd);
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
  flushLog();
  record->getId( id );

  printf( "LogManager: shutdown\n" );

  return 0;
}



