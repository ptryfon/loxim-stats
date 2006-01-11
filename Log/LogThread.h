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
#include "../TransactionManager/Transaction.h"
#include "LogRecord.h"
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

using namespace Store;
using namespace TManager;


namespace Logs
{
  class LogThread
  {
    protected:
    queue< LogRecord * > queue;
    pthread_t thread;
    bool threadFlag;
    pthread_cond_t threadFlagCV;
    pthread_mutex_t threadFlagMutex;
    int fileDes;

    static void *threadMain( void *arg );

    int pop( LogRecord *&record );

    public:
    LogThread( int _fileDes );
    ~LogThread() { flush(); }
    int push( LogRecord *record );
    void main();
    int flush();
  };

};

#endif


