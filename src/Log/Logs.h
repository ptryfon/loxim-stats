#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

namespace Logs
{
  class LogManager;
}

#include <string>
#include <stdio.h>
#include <Errors/ErrorConsole.h>
#include <Store/Store.h>
#include <Log/LogThread.h>
#include <Log/LogRecord.h>
#include <Config/SBQLConfig.h>

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
    std::string logFilePath;
    bool crashRecoveryInProgress;
    static bool cleanClosed;

    //jesli tid == NULL to do logow nie jest nic zapisywane
    void pushLogable( int tid, LogRecord *record);

    public:

    std::string getLogFilePath() { return logFilePath; }

    LogManager() { ec = &ErrorConsole::get_instance(EC_LOG); crashRecoveryInProgress = false; }

    int init();
    int start( StoreManager *store );
    static int checkForBackup();
    bool getCrashRecoveryInProgress() { return crashRecoveryInProgress; }

	/**
	 * Mowi czy ostatnio serwer zostal czysto zamkniety i czy przed aktualnym uruchomieniem nie bylo potrzebne odtwarzanie bazy
	 */
	static bool isCleanClosed() {return cleanClosed;}

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
    int write( int tid, LogicalID *lid, std::string name, DataValue *oldVal, DataValue *newVal, unsigned &id, bool newLID = false );

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

    /**
     * Zwraca biezaca wartosc zegara logicznego.
     */
    unsigned int getLogicalTimerValue();

    int destroy();
  };

};

#endif


