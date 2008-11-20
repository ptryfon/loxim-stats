#include <Log/LogIO.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


int LogIO::readInt( int fileDes, int &result )
{
  unsigned int n = 0;

  while( n < sizeof( result ) )
  {
    int size = ::read( fileDes, ((char *) &result)+n, sizeof( result )-n );
    if( !size ) return UNEXPECTED_EOF_ERROR;
    n += size;
    if( size == -1 ) return errno;
  }

  return 0;
}

int LogIO::writeInt( int fileDes, int val )
{
  return ( ::write( fileDes, &val, sizeof( val ) ) < (int) sizeof( val ) ) ? errno : 0;
}

int LogIO::getFilePos( int fileDes, off_t &result )
{
  result = ::lseek( fileDes, 0, SEEK_CUR );

  return ( result < 0 ) ? errno : 0;
}

int LogIO::readLogicalID( LogicalID *&lid, int fileDes, StoreManager* sm )
{
  string s;
  int errCode;

  if( ( ( errCode = readString( fileDes, s ) ) ) ) return errCode;
  if(s.empty())
  {
    lid=NULL;
  }
  else
  {
    if( sm )
      sm->logicalIDFromByteArray((unsigned char*) (s.data()), lid);
  }
  return 0;
}

int LogIO::writeLogicalID( LogicalID *lid, int fileDes )
{
  int result = 0;

  //gdy lid = NULL zapisujemy długość łańcucha = 0
  if( lid == NULL)
    return writeInt(fileDes, 0);


  Serialized serial1 = lid->serialize();
  string lidS = string( (const char*) serial1.bytes, serial1.size );
  result = writeString( fileDes, (char *) lidS.c_str(), lidS.length() );

  return result;
}

int LogIO::readDataValue( DataValue *&dv, int fileDes, StoreManager* sm )
{
  string s;
  int errCode;

  if( ( errCode = readString( fileDes, s ) ) ) return errCode;

  if(s.empty())
  {
    dv=NULL;
  }
  else
  {
    if( sm )
      sm->dataValueFromByteArray(new TransactionID(-1, -1, (int*)NULL), (unsigned char*) (s.data()), dv);
  }
  return 0;
}

int LogIO::readTransactionIDVector( vector<int> *&tidVec, int fileDes )
{
  int errCode;
  int size;

  if( ( errCode = readInt( fileDes, size ) ) ) return errCode;

  tidVec = new vector<int>( size );

  for( int i = 0; i < size; i++ )
  {
    int tid;

    errCode = readTransactionID( tid, fileDes );

    if( errCode ) return errCode;

    tidVec->operator[](i) = tid;
  }

  return 0;
}

int LogIO::writeTransactionIDVector( vector< int > *tidVec, int fileDes )
{
  int errCode;

  if( ( errCode = writeInt( fileDes, tidVec->size() ) ) ) return errCode;
  for( unsigned i = 0; i < tidVec->size(); i++ )
  {
    errCode = writeTransactionID( tidVec->operator[](i), fileDes );
    if( errCode ) return errCode;
  }

  return 0;
}

int LogIO::readString( int fileDes, string &str )
{
  int errCode;
  unsigned int n = 0;
  int len;
  char *buffer;

  if( ( errCode = readInt( fileDes, len ) ) ) return errCode;

  buffer = new char[len];

  while( n < (unsigned) len )
  {
    int size = ::read( fileDes, buffer+n, len-n );
    if( !size ) return UNEXPECTED_EOF_ERROR;
    n += size;
    if( size == -1 ) return errno;
  }

  str.clear();
  str.append(buffer, len);
  delete[] buffer;

  return 0;
}

int LogIO::writeString( int fileDes, const char *buffer, unsigned len )
{
  int errCode;
  unsigned int n = 0;

  if( ( errCode = writeInt( fileDes, len ) ) ) return errCode;

  while( n < len )
  {
    int size = ::write( fileDes, buffer+n, len-n );
    if( !size ) return UNEXPECTED_EOF_ERROR;
    n += size;
    if( size == -1 ) return errno;
  }

  return 0;
}




