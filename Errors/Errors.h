#ifndef _ERRORS_H
#define _ERRORS_H

#include <string>
#include <errno.h>

using namespace std;

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
#define ErrIndexes		0x4200000
#define ErrAllModules		0xFF00000

// error codes 1-255 (0x01 - 0xFF) reserved for system errno.h
#define ENoError		0x0

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
#define ENotUniqueNameList 0xB01

// Executor
#define ENumberExpected		0xC00
#define EBoolExpected		0xD00
#define ERefExpected		0xE00
#define EBagOfRefExpected	0xE10
#define EOtherResExp		0xF00
#define EUnknownValue		0x1000
#define EUnknownNode		0x1100
#define EQEmptySet		0x1200
#define EDivBy0			0x1300
#define EQEUnexpectedErr	0x1400
#define EEvalStopped		0x1500
#define EProcNotSingle		0x1600
#define EBadViewDef		0x1700
#define EBadBindName		0x1800
#define EOperNotDefined		0x1900
#define ENotUniqueClassName 0x1910
#define ENoClassDefFound	0x1920
#define EOneResultExpected	0x1930
#define ENotUniqueInterfaceName 0x1940

// Config
#define ENotInit		0x1A00
#define ENoValue		0x1B00
#define EBadValue		0x1C00

// Server
#define EReceive		0x1D00
#define EParse			0x1E00
#define EExecute		0x1F00
#define ESerialize		0x2000
#define ESend			0x2100
#define EBadResult		0x2200
#define EClientLost		0x2300

// Transaction
#define EDeadlock		0x2400
#define ESemaphoreInit		0x2500
#define EUpgradeLock		0x2600
#define EMutexInit		0x2700

// plug for h_errno from gethostbyname(3)
#define ENoHost			0x2800
#define EUnknownPackage		0x2900

// Indexes
#define EIndexExists	0x2A00
#define EFieldIndexed	0x2B00
#define ENoIndex		0x2C00

// stopper
#define EUnknown		0x2D00

string *SBQLstrerror(int error);
}

#endif
