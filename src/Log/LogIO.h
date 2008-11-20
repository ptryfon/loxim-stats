#ifndef __LOG_IO_H__
#define __LOG_IO_H__

namespace Logs
{
  class LogIO;
}

#include <stdio.h>
#include <Store/Store.h>

using namespace Store;


namespace Logs
{

#define UNEXPECTED_EOF_ERROR             100001

  class LogIO
  {
    public:
    // TODO
    static int readTransactionID( int &tid, int fileDes )
    {
      int id;
      int errCode;

      if( ( ( errCode = readInt( fileDes, id ) ) ) ) return errCode;
      tid = id;

      return 0;
    }
    static int writeTransactionID( int tid, int fileDes ) { return writeInt( fileDes, tid ); }
    static int readLogicalID( LogicalID *&lid, int fileDes, StoreManager* sm );
    static int writeLogicalID( LogicalID *lid, int fileDes );
    static int readDataValue( DataValue *&dv, int fileDes, StoreManager* sm );
    static int readTransactionIDVector( vector< int > *&tidVec, int fileDes );
    static int writeTransactionIDVector( vector< int > *tidVec, int fileDes );

    /// Odczyt/zapis jednej wartosci typu `int`
    static int readInt( int fileDes, int &result );
    static int writeInt( int fileDes, int val );
    static int readString( int fileDes, string &str );
    static int writeString( int fileDes, const char *buffer, unsigned len );//czemu buffer nie jest const char*??

    /// Zwraca wskaznik biezacej pozycji w pliku
    static int getFilePos( int fileDes, off_t &result );
  };

};

#endif

