#ifndef _ERRORS_H
#define _ERRORS_H

#include <errno.h>

// Sposob uzycia:
// return ENoFile | ErrLogs
// jesli istnieje kod bledu w errno.h nalezy go uzywac:
// return ENOSPC | ErrLogs

namespace Errors {
#define ErrBackup		 0x100000
#define	ErrConfig		 0x200000
#define	ErrDriver		 0x400000
#define	ErrErrors		 0x800000
#define	ErrLockMgr		0x1000000
#define	ErrLogs			0x1200000
#define	ErrQExecutor		0x1400000
#define	ErrQParser		0x1800000
#define	ErrSBQLCli		0x2000000
#define	ErrServer		0x2200000
#define	ErrStore		0x2400000
#define	ErrTManager		0x2800000
#define ErrTCPProto		0x4000000
#define ErrAllModules		0xFF00000

// error codes 1-255 (0x01 - 0xFF) reserved for system errno.h

#define ENoFile			0x100
//- Nieobslugiwany/niewlasciwy typ wartosci w CREATE
#define	EBadType		0x200
//- Blad podczas tworzenia obiektu
#define	EObjCreate		0x300
//- Blad podczas dodawania korzenia
#define	ERootAdd		0x400
//- Nieznany typ wezla drzewa zapytan
#define	ENoType			0x500
//- Proba pobrania (getResult) QueryResultu z pustego zbioru
#define EEmptySet		0x600
// blad pobierania korzenia
#define EGetRoot		0x700

// Store
#define EBadFile		0x800
#define EPageNotValid		0x900
#define EPageNotAquired		0xA00

// Parse error
#define ENotParsed		0xB00

// Executor

// jesli operacja arytmetyczna napotkala nie int/double
#define ENumberExpected		0xC01
// jesli operacje boolowskie napotkaja nie bool
#define EBoolExpected		0xC02
// jesli potrzebne jest LogicalID a jest cos innego
#define ERefExpected		0xC03
// pozostae przypadki niezgodnosci typow
#define EOtherResExp		0xC04
// kiedy otrzymalismy Value typu innego niz zdefiniowane w Store
#define EUnknownValue		0xC05
// kiedy nie wiemy jak obsluzyc dany wezel z drzewa od parsera
#define EUnknownNode		0xC06
// kiedy chcemy pobrac wartosc z pustego zbioru/stosu
#define EQEmptySet		0xC07
// blad dzielenia przez 0
#define EDivBy0			0xC08
// wszystkie pozostale sytuacje bledne (te najrzadsze)
#define EQEUnexpectedErr	0xC09

// Config
#define ENotInit		0x1000
#define ENoValue		0x1100
#define EBadValue		0x1200

// Server
#define EReceive		0x1300
#define EParse			0x1400
#define EExecute		0x1500
#define ESerialize		0x1600
#define ESend			0x1700
#define EBadResult		0x1800
#define EClientLost		0x1900

// Transaction
#define EDeadlock		0x2002
#define ESemaphoreInit		0x2004
#define EUpgradeLock		0x2006
#define EMutexInit		0x2008

// plug for h_errno from gethostbyname(3)
#define ENoHost			0x1A00

// stopper
#define EUnknown		0x1B00
}

#endif
