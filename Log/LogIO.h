#ifndef __LOG_IO_H__
#define __LOG_IO_H__

namespace Logs
{
  class LogIO;
}

#include <stdio.h>
#include "../Store/Store.h"
#include "../TransactionManager/Transaction.h"

using namespace Store;
using namespace TManager;


namespace Logs
{

#define UNEXPECTED_EOF_ERROR             100001

  class LogIO
  {
    public:
    // TODO
    static int readTransactionID( TransactionID *&tid, int fileDes )
    {
      int id;
      int errCode;

      if( ( ( errCode = readInt( fileDes, id ) ) ) ) return errCode;
      tid = new TransactionID( id );

      return 0;
    }
    static int writeTransactionID( TransactionID *tid, int fileDes ) { return writeInt( fileDes, tid->getId() ); }
    static int readLogicalID( LogicalID *&lid, int fileDes )
    {
      int id;
      int errCode;

      if( ( ( errCode = readInt( fileDes, id ) ) ) ) return errCode;
      // TODO: jak utworzyc LogicalID?

      return 0;
    }
    static int writeLogicalID( LogicalID *lid, int fileDes ) { return writeInt( fileDes, lid->toInteger() ); }
    static int readDataValue( DataValue *&dv, int fileDes );
    static int writeDataValue( DataValue *dv, int fileDes );
    static int readTransactionIDVector( vector< TransactionID *> *&tidVec, int fileDes );
    static int writeTransactionIDVector( vector< TransactionID *> *tidVec, int fileDes );

    /// Odczyt/zapis jednej wartosci typu `int`
    static int readInt( int fileDes, int &result );
    static int writeInt( int fileDes, int val );
    static int readString( int fileDes, string &str );
    static int writeString( int fileDes, char *buffer, unsigned len );

    /// Zwraca wskaznik biezacej pozycji w pliku
    static int getFilePos( int fileDes, off_t &result );
  };

};

#endif

