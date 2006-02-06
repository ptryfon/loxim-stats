#ifndef __BACKUP_H__
#define __BACKUP_H__

#include <string>
#include <string.h>

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

  int copyFile( string fromPath, string toPath );
  int eraseFile( string path );

  /**
   * Tworzy backup bazy danych w pliku o nazwie pobranym z Configa.
   */
  int makeBackup()
  {
    return
      copyFile( dbDefaultPath, backupDefaultPath ) ||
      copyFile( dbMapPath, backupMapPath ) ||
      copyFile( dbRootsPath, backupRootsPath );
  }

  /**
   * Odtwarza baze danych z backupu i logow.  Zakladamy ze przez ten czas
   * baza nie wykonuje zadnych innych dzialan.
   */
  int restore()
  {
    return
      copyFile( backupDefaultPath, dbDefaultPath ) ||
      copyFile( backupMapPath, dbMapPath ) ||
      copyFile( backupRootsPath, dbRootsPath ) ||
      eraseFile( logsPath );
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
