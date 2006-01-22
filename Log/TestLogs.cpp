#include <stdio.h>
#include "Logs.h"
#include "../Store/DBDataValue.h"
#include "../Store/DBLogicalID.h"
#include "../Store/DBStoreManager.h"
#include "LogsGlobals.cpp"


using namespace Store;


int main( int argc, char *argv[] )
{
  Logs::LogManager logManager;
  unsigned id;
  TransactionID *tid = new TransactionID( 1234 );

  logManager.init();
  logManager.beginTransaction( tid, id );

  // write (stary i nowy istnieje)
  {
    DataValue *oldDataValue = new DBDataValue( "Stary Mis" );
    DataValue *newDataValue = new DBDataValue( "Nowy Mis" );

    logManager.write( tid, new DBLogicalID( 57 ), "mis", oldDataValue, newDataValue, id );
  }

  // write (stary nie istnieje)
  logManager.write( tid, new DBLogicalID( 58 ), "mis2", 0, new DBDataValue( "Nowy Mis2" ), id );

  // write (nowy nie istnieje)
  logManager.write( tid, new DBLogicalID( 59 ), "mis3", new DBDataValue( "Stary Mis3" ), 0, id );

  // write (stary i nowy nie istnieje)
  logManager.write( tid, new DBLogicalID( 59 ), "mis3", 0, 0, id );

  //logManager.rollbackTransaction( tid, new DBStoreManager(), id );

  logManager.shutdown( id );
  ::sleep( 5 );
  logManager.destroy();
}
