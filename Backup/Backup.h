#ifndef __BACKUP_H__
#define __BACKUP_H__

#include <string>
#include <string.h>

#include "../Config/SBQLConfig.h"

using namespace std;
using namespace Config;

// nazwy opcji w pliku konfiguracyjnym
#define DBPATH_KEY "dbpath"
#define BACKUP_PATH_KEY "backuppath"
#define LOGS_PATH_KEY "logspath"
#define LOCK_PATH_KEY "lockpath"

class BackupManager
{
  protected:
  string configPath;
  int verboseLevel;
  SBQLConfig *config;
  string dbPath;
  string backupPath;
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
    errno = 0;
  }

  int init()
  {
    int errCode = 0;

    config->init( configPath );

    if( ( errCode = config->getString( DBPATH_KEY, dbPath ) ) ) return errCode;
    if( ( errCode = config->getString( BACKUP_PATH_KEY, backupPath ) ) ) return errCode;
    if( ( errCode = config->getString( LOGS_PATH_KEY, logsPath ) ) ) return errCode;
    if( ( errCode = config->getString( LOCK_PATH_KEY, lockPath ) ) ) return errCode;

    errCode = lock( config );

    return errCode;
  }

  int lock( SBQLConfig *config );
  int unlock( SBQLConfig *config );

  int copyFile( string fromPath, string toPath );
  int eraseFile( string path );

  /**
   * Tworzy backup bazy danych w pliku o nazwie pobranym z Configa.
   */
  int makeBackup() { return copyFile( dbPath, backupPath ); }

  /**
   * Odtwarza baze danych z backupu i logow.  Zakladamy ze przez ten czas
   * baza nie wykonuje zadnych innych dzialan.
   */
  int restore()
  {
    copyFile( backupPath, dbPath );
    eraseFile( logsPath );
    return 0;
  }

  string dump();

  int done()
  {
    unlock( config );
    delete config;

    return 0;
  }
};

int main( int argc, char *argv[] );

#endif
