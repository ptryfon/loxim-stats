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
// - unknown unOp node
#define EUNodeUNOP		0xC00
// - unknown algOp node
#define EUNodeALGOP		0xD00
// - wrong algOp (arithmetical) argument
#define EBadALGOPArg		0xE00
// - wrong deref argument
#define EBadDerefArg		0xF00

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
#define EClientLost		0x1A00

// plug for h_errno from gethostbyname(3)
#define ENoHost			0x1900
}

#endif
