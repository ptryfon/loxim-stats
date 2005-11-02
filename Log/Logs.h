#ifndef __LOGS_H__
#define __LOGS_H__

namespace Logs
{
  class LogManager;
}

#include <string>
#include <stdio.h>
#include "../Store/Store.h"
#include "../TransactionManager/transaction.h"

using namespace Store;
using namespace TManager;


namespace Logs
{

  class LogManager
  {
    public:

    LogManager() {}
    
    int init() { printf( "LogManager: initialize()\n" ); return 0; }
    
    /**
     * Zapisuje do dziennika <BEGIN tid>.
     */
    int beginTransaction( const TransactionID *tid ) { printf( "LogManager: beginTransaction\n"); return 0; }

    /**
     * Nakazuje zapisac w logach informacje o starej i nowej wartosci
     * zmodyfikowanego atrybutu.
     */
    int write( const TransactionID *tid, const LogicalID *lid, const DataValue *oldVal, const DataValue *newVal )
    { printf( "LogManager: write\n" ); return 0; }

    /**
     * Rozpoczyna tworzenie bezkonfliktowego punktu kontrolnego.
     */
    int checkpoint( const TransactionID *tid ) { printf( "LogManager: checkpoint\n"); return 0; }

    /**
     * Konczy sie w chwili gdy wszystkie logi transakcji tid znajda sie na dysku;
     * wtedy dopiero mozna zaczac zrzucac brudnych stron do bazy na dysku.
     */
    int commitTransaction( const TransactionID *tid ) { printf( "LogManager: commitTransaction\n" ); return 0; }

    /**
     * Zapisuje do dziennika <ABORT tid>.
     */
    int rollbackTransaction( const TransactionID *tid ) { printf( "LogManager: rollbackTransaction\n" ); return 0; }

    int destroy() { printf( "LogManager: destroy()\n" ); return 0; }
  };

};

#endif
