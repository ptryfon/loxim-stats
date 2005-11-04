#ifndef __BACKUP_H__
#define __BACKUP_H__

#include <string>

using namespace std;

class BackupManager
{
  protected:
  string storePath;
  string backupPath;
  string logsDir;
  int verboseLevel;

  public:

  /**
   * @param aStorePath sciezka do pliku bazy danych.
   * @param aBackupPath sciezka do pliku z backupem bazy danych.
   * @param aLogsDir sciezka do katalogu przechowujacego logi bazy danych.
   * @param aVerbose poziom szczegolowosci wypisywanych komunikatow.
   */
  BackupManager(string aStorePath, string aBackupPath, string aLogsDir, int aVerboseLevel = 0 )
  {
    storePath = aStorePath;
    backupPath = aBackupPath;
    logsDir = aLogsDir;
    verboseLevel = aVerboseLevel;
  }

  /**
   * Tworzy backup bazy danych w pliku o nazwie pobranym z Configa.
   */
  int makeBackup() { printf( "BackupManager: makeBackup()\n%s", dump().c_str() ); return 0; }

  /**
   * Odtwarza baze danych z backupu i logow.  Zakladamy ze przez ten czas
   * baza nie wykonuje zadnych innych dzialan.
   */
  int restore() { printf( "BackupManager: restore()\n%s", dump().c_str() ); return 0; }

  string dump();
};

int main( int argc, char *argv[] );

#endif
