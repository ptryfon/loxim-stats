#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

namespace Logs
{
  class LogManager;
}

#include <string>
#include <stdio.h>
#include "../Errors/ErrorConsole.h"
#include "../Store/Store.h"
#include "LogThread.h"
#include "LogRecord.h"
#include "../Config/SBQLConfig.h"

using namespace Errors;
using namespace Store;
using namespace Config;


namespace Logs
{

#define LOG_UNINITIALIZED_LOG_THREAD 200004

  class LogManager
  {
    protected:
    LogThread *logThread;
    int fileDes; // deskryptor pliku dziennika
    ErrorConsole *ec;
    SBQLConfig *config;
    string logFilePath;
    bool crashRecoveryInProgress;

    //jesli tid == NULL to do logow nie jest nic zapisywane
    void pushLogable( int tid, LogRecord *record);

    public:

    string getLogFilePath() { return logFilePath; }

    LogManager() { ec = new ErrorConsole("Log"); crashRecoveryInProgress = false; }

    int init();
    int start( StoreManager *store );
    bool getCrashRecoveryInProgress() { return crashRecoveryInProgress; }

    /**
    Robi flush Logów;
    */
    int flushLog();

    /**
     * Zapisuje do dziennika <BEGIN tid>.
     */
    int beginTransaction( int tid, unsigned &id );

    /**
     * Nakazuje zapisac w logach informacje o starej i nowej wartosci
     * zmodyfikowanego atrybutu.
     */
    int write( int tid, LogicalID *lid, string name, DataValue *oldVal, DataValue *newVal, unsigned &id );

    /**
     * Rozpoczyna tworzenie bezkonfliktowego punktu kontrolnego.
     */
    int checkpoint( vector<int> *tids, unsigned &id );

    /**
     * Konczy tworzenie bezkonfliktowego punktu kontrolnego.
     */
    int endCheckpoint( unsigned &id );

    /**
     * Konczy sie w chwili gdy wszystkie logi transakcji tid znajda sie na dysku;
     * wtedy dopiero mozna zaczac zrzucac brudnych stron do bazy na dysku.
     */
    int commitTransaction( int tid, unsigned &id );

    /**
    * Zapisuje tylko znak rollback (dla CrashRecovery)
    */
    int printRollbackSign(int tid);

    /**
     * Zapisuje do dziennika <ABORT tid>.
     */
    int rollbackTransaction( int tid, StoreManager *sm, unsigned &id );

    /**
     * Zapisuje do dziennika <SHUTDOWN> ("poprawnie zamknieto baze danych").
     */
    int shutdown( unsigned &id );

    /**
     * Nakazuje zapisac wlogach informacje o nowym korzeniu (root).
     */
    int addRoot( int tid, LogicalID *lid, unsigned &id );

    /**
     * Nakazuje zapisac wlogach informacje o nowym korzeniu (root).
     */
    int removeRoot( int tid, LogicalID *lid, unsigned &id );

    int destroy();
  };

};

#endif


