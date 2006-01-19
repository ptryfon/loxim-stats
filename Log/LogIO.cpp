#include "LogIO.h"
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


int LogIO::readInt( int fileDes, int &result )
{
  unsigned int n = 0;

  while( n < sizeof( result ) )
  {
    unsigned size = ::read( fileDes, ((char *) &result)+n, sizeof( result )-n );
    if( !size ) return UNEXPECTED_EOF_ERROR;
    n += size;
    if( errno ) return errno;
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
    return sm->logicalIDFromByteArray((unsigned char*) (s.data()), s.length(), &lid);
  }
  return 0;
}

int LogIO::writeLogicalID( LogicalID *lid, int fileDes ) 
{ 
  unsigned char *buffer;
  int len;
  int result;
  
  //gdy lid = NULL zapisujemy długość łańcucha = 0
  if( lid == NULL)
    return writeInt(fileDes, 0);
    
  lid->toByteArray( 0, &len );
  buffer = new unsigned char[len];
  lid->toByteArray( &buffer, &len );
  result = writeString( fileDes, (char *) buffer, len );
  delete buffer;

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
    return sm->dataValueFromByteArray((unsigned char*) (s.data()), s.length(), &dv);
  }
  return 0;
}

int LogIO::writeDataValue( DataValue *dv, int fileDes )
{
  unsigned char *buffer;
  int len;
  int result;

  //gdy dv = NULL zapisujemy długość łańcucha = 0
  if( dv == NULL)
    return writeInt(fileDes, 0);
    
  dv->toByteArray( 0, &len );
  buffer = new unsigned char[len];
  dv->toByteArray( &buffer, &len );
  result = writeString( fileDes, (char *) buffer, len );
  delete buffer;

  return result;
}

int LogIO::readTransactionIDVector( vector< TransactionID *> *&tidVec, int fileDes )
{
  int errCode;
  int size;

  if( ( errCode = readInt( fileDes, size ) ) ) return errCode;

  tidVec = new vector< TransactionID *>( size );

  for( int i = 0; i < size; i++ )
  {
    TransactionID *tid;

    errCode = readTransactionID( tid, fileDes );

    if( errCode ) return errCode;

    tidVec->operator[](i) = tid;
  }

  return 0;
}

int LogIO::writeTransactionIDVector( vector< TransactionID *> *tidVec, int fileDes )
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

  while( n < len )
  {
    unsigned size = ::read( fileDes, buffer+n, len-n );
    if( !size ) return UNEXPECTED_EOF_ERROR;
    n += size;
    if( errno ) return errno;
  }

  str.clear();
  str.append(buffer, len);
  delete buffer;

  return 0;
}

int LogIO::writeString( int fileDes, char *buffer, unsigned len )
{
  int errCode;
  unsigned int n = 0;

  if( ( errCode = writeInt( fileDes, len ) ) ) return errCode;

  while( n < len )
  {
    unsigned size = ::write( fileDes, buffer+n, len-n );
    if( !size ) return UNEXPECTED_EOF_ERROR;
    n += size;
    if( errno ) return errno;
  }

  return 0;
}




