#include <stdio.h>

class LogManager
{
  /**
   * Zapisuje do dziennika <BEGIN tid>.
   */
  void beginTransaction( TransactionID tid ) { printf( "LogManager: beginTransaction\n"); }

  /**
   * Nakazuje zapisac w logach informacje o starej i nowej wartosci
   * zmodyfikowanego atrybutu.
   */
  void write( TransactionID tid, LogicalID lid, Value oldVal, Value newVal ) { printf( "LogManager: write\n" ); }

  /**
   * Rozpoczyna tworzenie bezkonfliktowego punktu kontrolnego.
   */
  void checkpoint( TransactionID tid ) { printf( "LogManager: checkpoint\n"); }

  /**
   * Konczy sie w chwili gdy wszystkie logi transakcji tid znajda sie na dysku;
   * wtedy dopiero mozna zaczac zrzucac brudnych stron do bazy na dysku.
   */
  void commitTransaction( TransactionID tid ) { printf( "LogManager: commitTransaction\n" ); }

  /**
   * Zapisuje do dziennika <ABORT tid>.
   */
  void rollbackTransaction( TransactionID tid ) { printf( "LogManager: rollbackTransaction\n" ); }
};
