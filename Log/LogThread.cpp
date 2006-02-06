#include "LogThread.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>


/* LogThread class */

void *LogThread::threadMain( void *arg )
{
  LogThread *t = (LogThread*) arg;

  t->main();

  return NULL;
}

int LogThread::pop( LogRecord *&record )
{
  ::pthread_mutex_lock( &threadFlagMutex );
  while( queue.empty() )
  {
    int err = ::pthread_cond_wait( &threadFlagCV, &threadFlagMutex );
    if( err ) return err;
  }
  record = queue.front();
  queue.pop();
  ::pthread_mutex_unlock( &threadFlagMutex );

  return 0;
}

LogThread::LogThread( int _fileDes ) : fileDes( _fileDes )
{
  counter = 0;
  ::pthread_mutex_init( &threadFlagMutex, NULL );
  ::pthread_cond_init( &threadFlagCV, NULL );
  threadFlag = false;
  ::pthread_create( &thread, NULL, &threadMain, this );
}

int LogThread::push( LogRecord *record )
{
  ::pthread_mutex_lock( &threadFlagMutex );
  queue.push( record );
  counter++;
  ::pthread_cond_signal( &threadFlagCV );
  ::pthread_mutex_unlock( &threadFlagMutex );

  return 0;
}

void LogThread::main() {
  int errCode;
  LogRecord *logRecord;

  sigset_t mask;

  sigemptyset( &mask );
  sigaddset( &mask, SIGINT );
  pthread_sigmask( SIG_BLOCK, &mask, NULL );

  while( !( errCode = pop( logRecord ) ) )
  {
    if( ( errCode = LogRecord::writeLogRecord( logRecord, fileDes ) ) )
    {
      printf( "LogThread err_code: %d (%s)\n", errCode, strerror( errCode ) );
      counter = 0;
      return;
    }

    counter--;
    delete logRecord;

    ::pthread_mutex_lock( &threadFlagMutex );
    if( queue.empty() )
      ::pthread_cond_signal( &threadFlagCV );
    ::pthread_mutex_unlock( &threadFlagMutex );

  }
}

int LogThread::flush() {
  int err = 0;

  while( counter > 0 )
    ::sleep( 1 );

  return err;
}

