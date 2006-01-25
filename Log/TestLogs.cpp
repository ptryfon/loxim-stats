#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include "Logs.h"
#include "../Store/DBDataValue.h"
#include "../Store/DBLogicalID.h"
#include "../Store/DBStoreManager.h"
#include "LogsGlobals.cpp"


using namespace Store;
using namespace Logs;

void printLog(StoreManager* sm)
{
  int fd;
  int i=0;
  int err=0;
  LogRecord* lr;
  WriteRecord* wr;
  fd = ::open(LOG_FILE_PATH, O_RDONLY);
  while( (err = LogRecord::readLogRecordForward(lr, fd, sm)) == 0 && i++<50000)
  {
    int lt;
    lr->getType(lt);
    printf("\n|%d| \n",lt);
    if(lt == WRITE_LOG_REC_TYPE)
    {
      wr = (WriteRecord*) lr;
      if(wr->oldVal == NULL)
        printf("oldVal == NULL ");
      else
      {
        printf("oldVal = %s ",((wr->oldVal)->toString()).c_str());
      }
      printf("\n");
    }
    delete lr;
  }
  printf("\nerr=%d\n",err);
}


int main( int argc, char *argv[] )
{
  Logs::LogManager logManager;
  unsigned id;
  int tid = 1234;
  StoreManager* sm = new DBStoreManager();

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

  // write (stary i nowy istnieje)
  logManager.write( tid, new DBLogicalID( 59 ), "mis3", new DBDataValue( "Stary Mis4" ), new DBDataValue( "Nowy Mis4" ), id );


  logManager.rollbackTransaction( tid, sm, id );
  logManager.shutdown( id );
  logManager.flushLog();
  printLog(sm);
//  ::sleep( 2 );
  logManager.destroy();
}
