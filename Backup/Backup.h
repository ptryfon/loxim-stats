#ifndef __BACKUP_H__
#define __BACKUP_H__

#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "../Config/SBQLConfig.h"

using namespace std;
using namespace Config;

// nazwy opcji w pliku konfiguracyjnym
#define DB_DEFAULT_KEY "store_file_default"
#define DB_MAP_KEY "store_file_map"
#define DB_ROOTS_KEY "store_file_roots"

#define BACKUP_DEFAULT_KEY "backup_file_default"
#define BACKUP_MAP_KEY "backup_file_map"
#define BACKUP_ROOTS_KEY "backup_file_roots"

#define LOGS_PATH_KEY "logspath"
#define LOCK_PATH_KEY "lockpath"

class BackupManager
{
  protected:
  string configPath;
  int verboseLevel;
  SBQLConfig *config;
  SBQLConfig *serverConfig;
  SBQLConfig *logConfig;
  SBQLConfig *storeConfig;

  string dbDefaultPath;
  string dbMapPath;
  string dbRootsPath;
  string backupDefaultPath;
  string backupMapPath;
  string backupRootsPath;

  string logsPath;
  string lockPath;

  public:

  /**
   * @param aConfigPath sciezka do pliku konfiguracyjnego.
   * @param aVerbose poziom szczegolowosci wypisywanych komunikatow.
   */
  BackupManager(string aConfigPath, int aVerboseLevel = 0 )
  {
    configPath = aConfigPath;
    verboseLevel = aVerboseLevel;
    config = new SBQLConfig( "Backup" );
    serverConfig = new SBQLConfig( "Server" );
    logConfig = new SBQLConfig( "Log" );
    storeConfig = new SBQLConfig( "Store" );
  }

  int init()
  {
    int errCode = 0;

    config->init( configPath );
    serverConfig->init( configPath );
    logConfig->init( configPath );
    storeConfig->init( configPath );

    if( !errCode ) errCode = config->getString( BACKUP_DEFAULT_KEY, backupDefaultPath );
    if( !errCode ) errCode = config->getString( BACKUP_MAP_KEY, backupMapPath );
    if( !errCode ) errCode = config->getString( BACKUP_ROOTS_KEY, backupRootsPath );
    if( errCode )
      printf( "Aby Backup mogl dzialac, w pliku konfiguracyjnym musza w sekcji [Backup] istniec wpisy 'backup_file_default',\n"
              "'backup_file_map' i 'backup_file_roots' odpowiadajace wpisom 'store_file_default', 'store_file_map' i \n"
              "'store_file_roots' sekcji [Store]!\n" );

    if( !errCode ) errCode = storeConfig->getString( DB_DEFAULT_KEY, dbDefaultPath );
    if( !errCode ) errCode = storeConfig->getString( DB_MAP_KEY, dbMapPath );
    if( !errCode ) errCode = storeConfig->getString( DB_ROOTS_KEY, dbRootsPath );

    if( !errCode ) errCode = logConfig->getString( LOGS_PATH_KEY, logsPath );
    if( !errCode ) errCode = serverConfig->getString( LOCK_PATH_KEY, lockPath );

    if( !errCode ) errCode = lock( serverConfig );

    return errCode;
  }

  int lock( SBQLConfig *config );
  int unlock( SBQLConfig *config );

  bool fileExists( string filePath );

  int copyFile( string fromPath, string toPath );
  int eraseFile( string path );
  int readInt( int fileDes, int &result );
  int writeInt( int fileDes, int val );
  int writeString( int fileDes, const char *buffer, unsigned len );
  int getFilePos( int fileDes, off_t &result );
  int logRedoLogsRecord();
  string logGetLastRecord();

  /**
   * Tworzy backup bazy danych w pliku o nazwie pobranym z Configa.
   */
  int makeBackup()
  {
    int result = 0;

    // sprawdzamy czy na koncu pliku logow jest SHUTDOWN; jesli nie ma, to nie
    // pozwalamy na zrobienie backupu

    if( fileExists( logsPath ) )
    {
      string logRecord = logGetLastRecord();

      if( ( logRecord.length() == 0 ) || ( logRecord[4] != 0x7 ) )
      {
        printf( "Database was not stopped clear (there is no SHUTDOWN log record at end of log file)" );
        return -1;
      }
    }

    result =
      copyFile( dbDefaultPath, backupDefaultPath ) ||
      copyFile( dbMapPath, backupMapPath ) ||
      copyFile( dbRootsPath, backupRootsPath );

    // jesli istnieje plik logow, to usuwamy z niego wszystkie rekordy z
    // wyjatkiem ostatniego
    if( (result == 0) && fileExists( logsPath) )
    {
      string logRecord = logGetLastRecord();

      eraseFile( logsPath );

      int fileDes = ::open( logsPath.c_str(), O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR );

      writeString( fileDes, logRecord.c_str(), logRecord.length() );
      close(fileDes);
    }

    return result;
  }

  /**
   * Odtwarza baze danych z backupu i logow.  Zakladamy ze przez ten czas
   * baza nie wykonuje zadnych innych dzialan.
   */
  int restore()
  {
    int result = 0;

    if( fileExists( backupDefaultPath ) )
    {
      result =
        copyFile( backupDefaultPath, dbDefaultPath ) ||
        copyFile( backupMapPath, dbMapPath ) ||
        copyFile( backupRootsPath, dbRootsPath );
    } else
      printf( "There are no backup files to restore.\n" );

    if( fileExists( logsPath ) )
      logRedoLogsRecord();

    return result;
  }

  string dump();

  int done()
  {
    unlock( serverConfig );
    delete config;
    delete storeConfig;
    delete logConfig;
    delete serverConfig;

    return 0;
  }
};

int main( int argc, char *argv[] );

#endif
