#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include "LogIO.h"
#include "Logs.h"
#include "../Store/DBDataValue.h"
#include "../Store/DBLogicalID.h"
#include "../Store/DBStoreManager.h"
#include "LogsGlobals.cpp"
#include "../TransactionManager/Transaction.h"
#include "Errors/ErrorConsole.h"

using namespace Errors;
using namespace Store;
using namespace Logs;

void printLog(StoreManager* sm)
{
  int fd;
  off_t fpos;
  int err=0;
  LogRecord* lr;
  WriteRecord* wr;
  fd = ::open(LOG_FILE_PATH, O_RDONLY);

  ::lseek(fd, 0 ,SEEK_SET);
  LogIO::getFilePos( fd, fpos );
  printf("\npocztaek czytania : %ld\n",fpos);

  while( (err = LogRecord::readLogRecordForward(lr, fd, sm)) == 0 )
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
  close(fd);
  printf("\nerr=%d\n",err);
}


int main( int argc, char *argv[] )
{
  Logs::LogManager logManager;
  unsigned id;
  int tid = 1234;
  DBStoreManager* sm = new DBStoreManager();

  logManager.init();
  logManager.beginTransaction( tid, id );

  SBQLConfig *storeConfig = new SBQLConfig( "Store" );
  storeConfig->init("../Config/example.conf");
  ErrorConsole *ec = new ErrorConsole("Store");
  ec->init(1);
  sm->init( &logManager );
  sm->setTManager(TransactionManager::getHandle());
  sm->start();

  LogicalID *lid = new DBLogicalID( 58 );
  string name = "mis2";
  DataValue *dv = new DBDataValue( "Nowy Mis2" );
  ObjectPointer *op = sm->createObjectPointer( lid, name, dv );
  sm->createObject( NULL, name, dv, op );

//   // write (stary i nowy istnieje)
//   {
//     DataValue *oldDataValue = new DBDataValue( "Stary Mis" );
//     DataValue *newDataValue = new DBDataValue( "Nowy Mis" );
//     logManager.write( tid, new DBLogicalID( 57 ), "mis", oldDataValue, newDataValue, id );
//   }

  // write (stary nie istnieje)
  logManager.write( tid, new DBLogicalID( 58 ), "mis2", 0, new DBDataValue( "Nowy Mis2" ), id );

  // write (nowy nie istnieje)
  logManager.write( tid, new DBLogicalID( 59 ), "mis3", new DBDataValue( "Stary Mis3" ), 0, id );

//   // write (stary i nowy istnieje)
//   logManager.write( tid, new DBLogicalID( 59 ), "mis3", new DBDataValue( "Stary Mis4" ), new DBDataValue( "Nowy Mis4" ), id );


  logManager.rollbackTransaction( tid, sm, id );
  logManager.shutdown( id );
  logManager.flushLog();
  printLog(sm);
//  ::sleep( 2 );
  logManager.destroy();
}
