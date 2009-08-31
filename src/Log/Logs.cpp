#include <Log/Logs.h>
#include <Log/LogThread.h>
#include <Log/LogRecord.h>
#include <Log/Backup.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


namespace Logs {

/* LogManager class */

bool LogManager::cleanClosed = true;

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
//  config->init( "../Server/Server.conf" );
  if( ( errCode = config->getString( "logspath", logFilePath ) ) ) return errCode;

  debug_printf(*ec,  "LogsPath: %s\n", logFilePath.c_str() );

  // nie wykonujecie tej metody!
  // jedyne miejsce gdzie ec->init ma prawo wystapic to main()

  // no kurde! A gdzie jest wolane LogManager::init()? Nie w MAIN()?!
  // i skad ja w main() wezme ec, jesli to jest chronione pole LogManagera?
  //ec->init(2); // baggins, prosze - nie kasuj!

  // otwieramy plik z logami
  if( ( fileDes = ::open( logFilePath.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR ) ) < 0 ) return errno;

  ::lseek( fileDes, 0, SEEK_END );
  // odpalamy watek
  logThread = new LogThread( fileDes );

  {
    int fd = ::open( logFilePath.c_str(), O_RDONLY, S_IWUSR | S_IRUSR );

    if( fd < 0 ) return errno;
    LogRecord::initialize( fd );

    ::close( fd );
  }

  debug_print(*ec,  "LogManager: initialize()");

  return 0;
}

int LogManager::start( StoreManager *store )
{
  crashRecoveryInProgress = true;
  CrashRecovery *cr = new CrashRecovery();
  int errCode = 0;

  errCode = cr->init( this, store );
  if( errCode == 0 )
  {
    debug_printf(*ec,  "CrashRecovery::Recover BEGIN !\n" );
    errCode = cr->Recover();
    debug_printf(*ec,  "CrashRecovery::Recover END !\n" );
  }

  delete cr;

  crashRecoveryInProgress = false;
  return errCode;
}

/**
 * Sprawdzamy czy na koncu logow jest rekord SHUTDOWN;
 * jesli jest taki rekord (lub nie ma pliku logow), nie robimy NIC
 * w p.p. odtwarzamy backup (jesli jest takowy) i wstawiamy na koniec logow rekord REDO_LOGS
 */
int LogManager::checkForBackup()
{
  int fileDes;
  LogRecord *lr = NULL;
  int err;
  int result = 0;
  SBQLConfig *cfg = new SBQLConfig( "Log" );
  string logsPath;

  if( ( err = cfg->getString( "logspath", logsPath ) ) ) return err;

  // otwieramy plik logow
  fileDes = ::open( logsPath.c_str(), O_RDWR, S_IWUSR | S_IRUSR );

  if( fileDes < 0 )
  {
    if( errno == 2 ) // plik logow nie istnieje
      return 0;
    else // nie udalo sie otworzyc pliku logow
      return errno;
  }

  // sprawdzamy czy na koncu jest rekord SHUTDOWN
  if( ::lseek( fileDes, 0, SEEK_END ) < 0 ) return errno;
  if( (err = LogRecord::readLogRecordBackward(lr, fileDes, NULL)) == 0)
  {
    int type = 1;

    if( lr ) // jesli plik logow jest niepusty
      lr->getType( type );

    cleanClosed = true;
    if( type != SHUTDOWN_LOG_REC_TYPE )
    {
      int result;
      BackupManager manager; // na koncu logow umieszcza REDO_LOGS

      // odtwarzamy backup (jesli jest) i wstawiamy na koniec logow REDO_LOGS
      ::lseek( fileDes, 0, SEEK_END );
      LogRecord::initialize( fileDes );
      cleanClosed = false;
      printf( "Server wasn't stopped clear last time, restoring backup...\n" );
      manager.init();
      result = manager.restore();
      manager.done();
    }
  }
  delete lr;

  close( fileDes );

  return result;
}

int LogManager::destroy()
{
  LogRecord::initialize( 0 );
  delete logThread;

  ::close( fileDes );

  debug_print(*ec,  "LogManager: destroy()");

  return 0;
}

int LogManager::beginTransaction( int tid, unsigned &id )
{
  LogRecord *record = new BeginTransactionRecord( tid );
  record->getId( id );
  logThread->push( record );

  debug_print(*ec,  "LogManager: beginTransaction");

  return 0;
}

//jeśli tid == NULL to do logów nie jest nic zapisywane
int LogManager::write( int tid, LogicalID *lid, string name, DataValue *oldVal, DataValue *newVal, unsigned &id, bool newLID )
{
  LogRecord *record = new WriteRecord( tid, lid, name, oldVal, newVal, newLID );
  record->getId( id );
  pushLogable( tid, record );

  debug_print(*ec,  "LogManager: write");

  return 0;
}

int LogManager::addRoot( int tid, LogicalID *lid, unsigned &id )
{
  LogRecord *record = new AddRootRecord( tid, lid);
  record->getId( id );
  pushLogable( tid, record );

  debug_print(*ec,  "LogManager: addRoot");
  return 0;
}

int LogManager::removeRoot( int tid, LogicalID *lid, unsigned &id )
{
  LogRecord *record = new RemoveRootRecord( tid, lid);
  record->getId( id );
  pushLogable( tid, record );

  debug_print(*ec,  "LogManager: addRoot");
  return 0;
}

int LogManager::checkpoint( vector<int> *tids, unsigned &id )
{
  LogRecord *record = new CkptRecord( tids );
  record->getId( id );
  logThread->push_and_flush( record );

  debug_print(*ec,  "LogManager: checkpoint!!");

  return 0;
}

int LogManager::endCheckpoint( unsigned &id )
{
  LogRecord *record = new EndCkptRecord();
  record->getId( id );
  logThread->push_and_flush( record );

  debug_print(*ec,  "LogManager: end checkpoint");

  return 0;
}

int LogManager::commitTransaction( int tid, unsigned &id )
{
  //sk
  int err=0;
  
  LogRecord *record = new CommitRecord( tid );
  record->getId( id );
  logThread->push_and_flush( record );
  
  debug_print(*ec,  "LogManager: commitTransaction");

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
  record->getId( id );
  logThread->push( record );

  debug_print(*ec,  "LogManager: rollbackTransaction");

  return 0;
}

int LogManager::shutdown( unsigned &id )
{
  LogRecord *record = new ShutdownRecord();
  record->getId( id );
  logThread->push_and_flush( record );

  debug_print(*ec,  "LogManager: shutdown");

  return 0;
}

unsigned int LogManager::getLogicalTimerValue()
{
  return LogRecord::getIdSeq();
}

}

