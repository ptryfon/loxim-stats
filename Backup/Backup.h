#ifndef __BACKUP_H__
#define __BACKUP_H__

namespace Backup
{
  class BackupManager;
}

#include <string>
#include <stdio.h>


namespace Backup
{

  class BackupManager
  {
    public:

    BackupManager() {}

    int init() { printf( "BackupManager: init()\n" ); return 0; }

    /**
     * Tworzy backup bazy danych w pliku o nazwie pobranym z Configa.
     */
    int makeBackup() { printf( "BackupManager: makeBackup()\n" ); return 0; }

    /**
     * Odtwarza baze danych z backupu i logow.  Zakladamy ze przez ten czas
     * baza nie wykonuje zadnych innych dzialan.
     */
    int restore() { printf( "BackupManager: restore()\n" ); return 0; }

    int destroy() { printf( "BackupManager: destroy()\n" ); return 0; }
  };

};

#endif
