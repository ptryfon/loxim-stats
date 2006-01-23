#include "Backup.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/// Nazwa tego programu
static string programName;

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


string BackupManager::dump()
{
  return
    string( "      dbPath: " ) + dbPath + string( "\n" ) +
    string( "  backupPath: " ) + backupPath + string( "\n" ) +
    string( "    logsPath: " ) + logsPath + string( "\n" ) +
    string( "    lockPath: " ) + lockPath + string( "\n" ) +
    string( "  configPath: " ) + configPath + string( "\n" );
}
