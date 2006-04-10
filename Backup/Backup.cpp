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

static void printUsage( int exitCode, string message = "" )
{
  if( message.length() > 0 )
    printf( "ERROR: %s\n", message.c_str() );

  printf( "Usage: %s options\n", programName.c_str() );
  printf( "    -h --help        Shows this info\n"
          "    -c --config-file Config file path\n"
          "    -r --restore     Restore\n"
          "    -v --verbose     Verbose mode\n"
          "Option -c is obligatory.\n" );
  exit( exitCode );
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

int main( int argc, char *argv[] )
{
  int nextOption;
  string shortOptions = "hc:rv"; // Litery prawidlowych krotkich opcji
  const struct option longOptions[] = {
    { "help", 0, 0, 'h' },
    { "restore", 0, 0, 'r' },
    { "verbose", 0, 0, 'v' },
    { 0, 0, 0, 0 } // wymagane do zakonczenia tablicy
  };
  int verboseLevel = 0;
  bool restoreFlag = false;
  string configPath;
  int errCode = 0;

  programName = argv[0];

  do
  {
    nextOption = ::getopt_long( argc, argv, shortOptions.c_str(), longOptions, NULL );
    switch( nextOption )
    {
      case 'h': // help
        printUsage( 0 );

      case 'c': // config file path
        configPath = optarg;
        break;

      case 'r' : // restore flag
        restoreFlag = true;
        break;

      case 'v': // verbose
        verboseLevel = 1;
        break;

      case '?': // nieprawidlowa opcja
        printUsage( 1 );

      case -1: // koniec opcji
        break;

      default: // cos innego, niespodziewane
        abort();
    }
  } while( nextOption != -1 );

  if( configPath.length() == 0 )
    printUsage( 1 );

  BackupManager manager( configPath, verboseLevel );

  if( ( errCode = manager.init() ) )
    exit( errCode );

  if( restoreFlag ) manager.restore();
  else manager.makeBackup();

  manager.done();

  printf( "Done.\n" );
}


int BackupManager::readInt( int fileDes, int &result )
{
  unsigned int n = 0;

  while( n < sizeof( result ) )
  {
    int size = ::read( fileDes, ((char *) &result)+n, sizeof( result )-n );
    if( !size ) return 2000002;
    n += size;
    if( size == -1 ) return errno;
  }

  return 0;
}

int BackupManager::writeInt( int fileDes, int val )
{
  return ( ::write( fileDes, &val, sizeof( val ) ) < (int) sizeof( val ) ) ? errno : 0;
}

int BackupManager::writeString( int fileDes, const char *buffer, unsigned len )
{
  unsigned int n = 0;

  while( n < len )
  {
    int size = ::write( fileDes, buffer+n, len-n );
    if( !size ) return 200002;
    n += size;
    if( size == -1 ) return errno;
  }

  return 0;
}

int BackupManager::getFilePos( int fileDes, off_t &result )
{
  result = ::lseek( fileDes, 0, SEEK_CUR );

  return ( result < 0 ) ? errno : 0;
}

int BackupManager::logRedoLogsRecord()
{
  int timer = 0;
  int fileDes = ::open( logsPath.c_str(), O_RDWR, S_IWUSR | S_IRUSR );

  if( fileDes < 0 ) return errno;

  // ustalamy wartosc zegara logicznego
  ::lseek( fileDes, 0, SEEK_END ); // na koniec pliku
  if( ::lseek( fileDes, 0, SEEK_CUR ) >= 12 ) {
    int len; // rozmiar rekordu
    ::lseek( fileDes, - sizeof( int ), SEEK_CUR ); // o 4 bajty do tylu
    readInt( fileDes, len );
    ::lseek( fileDes, - (len-4), SEEK_CUR );
    readInt( fileDes, timer );
    timer++;
  }

  // zapisujemy rekord REDO_LOGS
  ::lseek( fileDes, 0, SEEK_END ); // na koniec pliku
  writeInt( fileDes, 10 ); // typ rekordu (REDO_LOGS)
  writeInt( fileDes, timer );
  writeInt( fileDes, 12 ); // dlugosc rekordu

  close( fileDes );

  return 0;
}

string BackupManager::logGetLastRecord()
{
  string result = "";
  int fileDes = ::open( logsPath.c_str(), O_RDONLY, S_IWUSR | S_IRUSR );
  int len; // rozmiar rekordu
  char *buffer;
  unsigned int n = 0;

  if( fileDes < 0 ) return "";

  ::lseek( fileDes, 0, SEEK_END ); // na koniec pliku
  if( ::lseek( fileDes, 0, SEEK_CUR ) < 12 ) { close(fileDes); return ""; }
  ::lseek( fileDes, - sizeof( int ), SEEK_CUR ); // o 4 bajty do tylu
  readInt( fileDes, len );
  ::lseek( fileDes, - len, SEEK_CUR );

  buffer = new char[len];
  while( n < (unsigned) len )
  {
    int size = ::read( fileDes, buffer+n, len-n );
    if( !size ) { close(fileDes); return ""; }
    n += size;
    if( size == -1 ) { close( fileDes); return ""; }
  }

  result = string(buffer, len);

  close( fileDes );

  return result;
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
    string( "    lockPath: " ) + lockPath + string( "\n" ) +
    string( "  configPath: " ) + configPath + string( "\n" );
}
