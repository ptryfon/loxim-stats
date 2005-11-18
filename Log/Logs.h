#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

namespace Logs
{
  class LogManager;
}

#include <string>
#include <stdio.h>
#include "../Store/Store.h"
#include "../TransactionManager/Transaction.h"
#include "LogThread.h"

using namespace Store;
using namespace TManager;


namespace Logs
{

// tymczasowo - pozniej sie wezmie z configa
#define LOG_FILE_PATH "szbd_logs"

  class LogManager
  {
    protected:
    LogThread *logThread;
    int fileDes; // deskryptor pliku dziennika

    public:

    LogManager() {}

    int init();

    /**
     * Zapisuje do dziennika <BEGIN tid>.
     */
    int beginTransaction( TransactionID *tid, unsigned &id );

    /**
     * Nakazuje zapisac w logach informacje o starej i nowej wartosci
     * zmodyfikowanego atrybutu.
     */
    int write( TransactionID *tid, LogicalID *lid, DataValue *oldVal, DataValue *newVal, unsigned &id );

    /**
     * Rozpoczyna tworzenie bezkonfliktowego punktu kontrolnego.
     */
    int checkpoint( vector<TransactionID *> *tids, unsigned &id );

    /**
     * Konczy tworzenie bezkonfliktowego punktu kontrolnego.
     */
    int endCheckpoint( unsigned &id );

    /**
     * Konczy sie w chwili gdy wszystkie logi transakcji tid znajda sie na dysku;
     * wtedy dopiero mozna zaczac zrzucac brudnych stron do bazy na dysku.
     */
    int commitTransaction( TransactionID *tid, unsigned &id );

    /**
     * Zapisuje do dziennika <ABORT tid>.
     */
    int rollbackTransaction( TransactionID *tid, unsigned &id );

    int destroy();
  };

};

#endif


