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
    static int readTransactionID( TransactionID *&tid, int fileDes ) { tid = NULL; return 0; }
    static int writeTransactionID( TransactionID *tid, int fileDes ) { printf( "Logs: writeTransactionID()\n" ); return 0; }
    static int readLogicalID( LogicalID *&lid, int fileDes ) { lid = NULL; return 0; }
    static int writeLogicalID( LogicalID *lid, int fileDes ) { printf( "Logs: writeLogicalID()\n" ); return 0; }
    static int readDataValue( DataValue *&dv, int fileDes ) { dv = NULL; return 0; }
    static int writeDataValue( DataValue *dv, int fileDes ) { printf( "Logs: writeDataValue()\n" ); return 0; }
    static int readTransactionIDVector( vector< TransactionID *> *&tidVec, int fileDes ) { tidVec = NULL; return 0; }
    static int writeTransactionIDVector( vector< TransactionID *> *tidVec, int fileDes ) { printf( "Logs: writeTransactionIDVector()\n" ); return 0; }

    /// Odczyt/zapis jednej wartosci typu `int`
    static int readInt( int fileDes, int &result );
    static int writeInt( int fileDes, int val );

    /// Zwraca wskaznik biezacej pozycji w pliku
    static int getFilePos( int fileDes, off_t &result );
  };

};

#endif

