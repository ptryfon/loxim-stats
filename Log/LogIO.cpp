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

