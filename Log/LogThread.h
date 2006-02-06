#ifndef __LOG_THREAD_H__
#define __LOG_THREAD_H__

namespace Logs
{
  class LogThread;
  class LogRecordQueue;
}

#include <string>
#include <stdio.h>
#include "../Store/Store.h"
#include "LogRecord.h"
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

using namespace Store;


namespace Logs
{
  class LogThread
  {
    protected:
    queue< LogRecord * > queue;
    pthread_t thread;
    bool threadFlag;
    pthread_cond_t threadFlagCV;
    pthread_cond_t threadFlagCV2;
    pthread_mutex_t threadFlagMutex;
    int fileDes;
    int counter;

    static void *threadMain( void *arg );

    int pop( LogRecord *&record );

    public:
    LogThread( int _fileDes );
    int push( LogRecord *record );
    void main();
    int flush();
  };

};

#endif


