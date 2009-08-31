#include <Log/LogThread.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

namespace Logs {

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
  while( counter == 0 )
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
  ::pthread_cond_init( &threadFlagCV2, NULL );
  threadFlag = false;
  ::pthread_create( &thread, NULL, &threadMain, this );
}

int LogThread::push( LogRecord *record, bool already_locked )
{
  if (!already_locked)
	  ::pthread_mutex_lock( &threadFlagMutex );
  queue.push( record );
  counter++;
  ::pthread_cond_signal( &threadFlagCV );
  if (!already_locked)
	  ::pthread_mutex_unlock( &threadFlagMutex );

  return 0;
}

int LogThread::push_and_flush(LogRecord *lr)
{
	::pthread_mutex_lock( &threadFlagMutex);
	int err = push(lr, true);
	if (err) {
		::pthread_mutex_unlock(&threadFlagMutex);
		return err;
	}
	err = flush(true);
	::pthread_mutex_unlock( &threadFlagMutex);
	return err;
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

    delete logRecord;

    ::pthread_mutex_lock( &threadFlagMutex );
    counter--;
    if( counter <= 0 )
    {
      ::pthread_cond_signal( &threadFlagCV2 );
      counter = 0;
    }
    ::pthread_mutex_unlock( &threadFlagMutex );

  }
}

int LogThread::flush(bool already_locked) {
  int err = 0;

  if (!already_locked)
	  ::pthread_mutex_lock( &threadFlagMutex );
  while( counter > 0 || err ) {
    err = ::pthread_cond_wait( &threadFlagCV2, &threadFlagMutex );
  }
  //sk
  if(fsync(fileDes) < 0) err = errno;
 
  if (!already_locked) 
	 ::pthread_mutex_unlock( &threadFlagMutex );

  return err;
}

}

