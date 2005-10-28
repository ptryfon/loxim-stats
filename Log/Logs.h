#ifndef __LOGS_H__
#define __LOGS_H__

#include <string>
#include <stdio.h>
#include "../Store/Store.h"
#include "../TransactionManager/transaction.h"

using namespace Store;


namespace Logs
{

  class LogManager
  {
    public:

    LogManager() {}
    
    int init( string logPath ) { printf( "LogManager: initialize( logPath=%s )\n", logPath.c_str() ); return 0; }
    
    /**
     * Zapisuje do dziennika <BEGIN tid>.
     */
    int beginTransaction( TransactionID tid ) { printf( "LogManager: beginTransaction\n"); return 0; }

    /**
     * Nakazuje zapisac w logach informacje o starej i nowej wartosci
     * zmodyfikowanego atrybutu.
     */
    int write( TransactionID tid, LogicalID lid, DataValue oldVal, DataValue newVal ) { printf( "LogManager: write\n" ); return 0; }

    /**
     * Rozpoczyna tworzenie bezkonfliktowego punktu kontrolnego.
     */
    int checkpoint( TransactionID tid ) { printf( "LogManager: checkpoint\n"); return 0; }

    /**
     * Konczy sie w chwili gdy wszystkie logi transakcji tid znajda sie na dysku;
     * wtedy dopiero mozna zaczac zrzucac brudnych stron do bazy na dysku.
     */
    int commitTransaction( TransactionID tid ) { printf( "LogManager: commitTransaction\n" ); return 0; }

    /**
     * Zapisuje do dziennika <ABORT tid>.
     */
    int rollbackTransaction( TransactionID tid ) { printf( "LogManager: rollbackTransaction\n" ); return 0; }

    int destroy() { printf( "LogManager: destroy()\n" ); return 0; }
  };

};

#endif
