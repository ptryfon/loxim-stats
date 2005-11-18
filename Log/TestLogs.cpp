#include <stdio.h>
#include "Logs.h"
#include "LogsGlobals.cpp"


int main( int argc, char *argv[] )
{
  Logs::LogManager logManager;
  unsigned id;

  logManager.init();
  logManager.beginTransaction( new TransactionID( 1234 ), id );
  ::sleep( 5 );
  logManager.destroy();
}
