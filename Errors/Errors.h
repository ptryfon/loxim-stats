#ifndef _ERRORS_H
#define _ERRORS_H

#include <errno.h>

// Sposob uzycia:
// return ENoFile | ErrLogs
// jesli istnieje kod bledu w errno.h nalezy go uzywac:
// return ENOSPC | ErrLogs

namespace Errors {
#define ErrBackup		 0x1000
#define	ErrConfig		 0x2000
#define	ErrDriver		 0x4000
#define	ErrErrors		 0x8000
#define	ErrLockMgr		0x10000
#define	ErrLogs			0x12000
#define	ErrQExecutor		0x14000
#define	ErrQParser		0x18000
#define	ErrSBQLCli		0x20000
#define	ErrServer		0x22000
#define	ErrStore		0x24000
#define	ErrTManager		0x28000
#define ErrTCPProto		0x40000
#define ErrAllModules		0xFF000

#define ENoFile			    0x1
//- Nieobslugiwany/niewlasciwy typ wartosci w CREATE
#define	EBadType		    0x2
//- Blad podczas tworzenia obiektu
#define	EObjCreate		    0x3
//- Blad podczas dodawania korzenia
#define	ERootAdd		    0x4
//- Nieznany typ wezla drzewa zapytan
#define	ENoType			    0x5
//- Proba pobrania (getResult) QueryResultu z pustego zbioru
#define EEmptySet		    0x6
// blad pobierania korzenia
#define EGetRoot		   0x10

// Store
#define EBadFile		    0x7
#define EPageNotValid		    0x8
#define EPageNotAquired		    0x9

// Parser
#define ENotParsed		    0xA
}

#endif
