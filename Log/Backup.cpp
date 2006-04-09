#include "Backup.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


/// Nazwa tego programu
static string programName;

bool BackupManager::fileExists( string filePath )
{
  int fd;
  bool result = true;

  fd = ::open( filePath.c_str(), O_RDONLY );

  if( fd < 0 )
    result = false;
  else
    close( fd );

  return result;
}

int BackupManager::copyFile( string fromPath, string toPath )
{
  int fdFrom;
  int fdTo;
  unsigned char buffer[65536];
  int size;

  printf( "Kopiuje '%s' do '%s'\n", fromPath.c_str(), toPath.c_str() );
  if( ( fdTo = ::open( toPath.c_str(), O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR ) ) < 0 ) {
    printf( "Nie moge otworzyc pliku '%s' do zapisu: %s\n", toPath.c_str(), strerror( errno ) );
    unlock( config );
    exit( 1 );
  }

  if( ( fdFrom = ::open( fromPath.c_str(), O_RDONLY ) ) < 0 ) {
    printf( "Nie moge otworzyc pliku '%s' do odczytu: %s\n", fromPath.c_str(), strerror( errno ) );
    unlock( config );
    exit( 1 );
  }

  while( ( size = ::read( fdFrom, buffer, 65536 ) ) > 0 )
  {
    int index = 0;

    while( index < size )
    {
      int sz = ::write( fdTo, buffer + index, size-index );

      if( sz < 0 )
      {
        printf( "Blad zapisu pliku '%s': %s\n", toPath.c_str(), strerror( errno ) );
        unlock( config );
        exit( 1 );
      }

      index += sz;
    }
  }

  if( size < 0 )
  {
    printf( "Blad odczytu pliku '%s': %s\n", fromPath.c_str(), strerror( errno ) );
    unlock( config );
    exit( 1 );
  }

  ::close( fdFrom );
  ::close( fdTo );
  printf( "Skopiowane.\n" );

  return 0;
}

int BackupManager::eraseFile( string path )
{
  printf( "Usuwam plik '%s'\n", path.c_str() );
  if( ( ::unlink( path.c_str() ) < 0 ) )
  {
    printf( "Nie moge usunac pliku '%s': %s\n", path.c_str(), strerror( errno ) );
  } else
    printf( "Skasowany.\n" );

  return 0;
}

int BackupManager::lock( SBQLConfig *config )
{
  string lockPath;
  int errCode = 0;
  struct stat statBuf;
  int fileDes;

  if( ( errCode = config->getString( LOCK_PATH_KEY, lockPath ) ) )
    exit( errCode );

  // sprawdzamy czy istnieje plik blokady

  if( ::stat( lockPath.c_str(), &statBuf ) == 0 ) { // istnieje - blad
    printf( "ERROR: nie mozesz uruchomic '%s' poniewaz prawdopodobnie dziala\n"
            "    program serwera baz danych.  Jesli jestes pewien ze nie dziala,\n"
            "    usun plik '%s'.\n", programName.c_str(), lockPath.c_str() );
    exit( 1 );
  } else { // tworzymy plik blokady
    if( ( fileDes = ::open( lockPath.c_str(), O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR ) ) < 0 )
    {
      printf( "Nie moge utworzyc pliku '%s': %s\n", lockPath.c_str(), strerror( errno ) );
      exit( 1 );
    }
  }
  // plik blokady utworzony
  errno = 0;

  ::close( fileDes );

  return errCode;
}

int BackupManager::unlock( SBQLConfig *config )
{
  string lockPath;
  int errCode = 0;

  // usuwamy plik blokady
  config->getString( LOCK_PATH_KEY, lockPath );

  if( ::unlink( lockPath.c_str() ) < 0 )
  {
    printf( "Nie moge usunac pliku blokady '%s': %s\n", lockPath.c_str(), strerror( errno ) );
    return errno;
  }

  // blokada usunieta

  return errCode;
}


string BackupManager::dump()
{
  return
    string( "      dbDefaultPath: " ) + dbDefaultPath + string( "\n" ) +
    string( "      dbMapPath: " ) + dbMapPath + string( "\n" ) +
    string( "      dbRootsPath: " ) + dbRootsPath + string( "\n" ) +
    string( "      backupDefaultPath: " ) + backupDefaultPath + string( "\n" ) +
    string( "      backupMapPath: " ) + backupMapPath + string( "\n" ) +
    string( "      backupRootsPath: " ) + backupRootsPath + string( "\n" ) +
    string( "    logsPath: " ) + logsPath + string( "\n" ) +
    string( "    lockPath: " ) + lockPath + string( "\n" );
}
