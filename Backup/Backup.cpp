#include "Backup.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>


/// Nazwa tego programu
static string programName;

static void printUsage( int exitCode, string message = "" )
{
  if( message.length() > 0 )
    printf( "ERROR: %s\n", message.c_str() );

  printf( "Usage: %s options\n", programName.c_str() );
  printf( "    -h --help          Shows this info\n"
          "    -f --database-file Database store path\n"
          "    -l --logs-dir dir  Database logs directory\n"
          "    -b --backup-file   Backup file path\n"
          "    -r --restore       Restore\n"
          "    -v --verbose       Verbose mode\n"
          "Options: -f -l -b are obligatory.\n" );
  exit( exitCode );
}

int main( int argc, char *argv[] )
{
  int nextOption;
  string shortOptions = "hf:l:b:rv"; // Litery prawidlowych krotkich opcji
  const struct option longOptions[] = {
    { "help", 0, 0, 'h' },
    { "database-file", 1, 0, 'f' },
    { "logs-dir", 1, 0, 'l' },
    { "backup-file", 1, 0, 'b' },
    { "restore", 0, 0, 'r' },
    { "verbose", 0, 0, 'v' },
    { 0, 0, 0, 0 } // wymagane do zakonczenia tablicy
  };
  string backupPath;
  string logsDir;
  string databasePath;
  int verboseLevel = 0;
  bool restoreFlag = false;

  programName = argv[0];

  do
  {
    nextOption = ::getopt_long( argc, argv, shortOptions.c_str(), longOptions, NULL );
    switch( nextOption )
    {
      case 'h': // help
        printUsage( 0 );

      case 'f': // database file
        databasePath = optarg;
        break;

      case 'b': // backup file
        backupPath = optarg;
        break;

      case 'l': // logs dir
        logsDir = optarg;
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

  if( backupPath.length() * logsDir.length() * databasePath.length() == 0 )
    printUsage( 1 );

  BackupManager manager( databasePath, backupPath, logsDir, verboseLevel );

  if( restoreFlag ) manager.restore();
  else manager.makeBackup();

  printf( "Done.\n" );
}


string BackupManager::dump()
{
  return
    string( "  storePath: " ) + storePath + string( "\n" ) +
    string( "  backupPath: " ) + backupPath + string( "\n" ) +
    string( "  logsDir: " ) + logsDir + string( "\n" );
}
