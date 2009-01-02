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
	enum{
		ErrBackup =			0x100000,
		ErrConfig =			0x200000,
		ErrDriver =			0x400000,
		ErrErrors =			0x800000,
		ErrLockMgr =			0x1000000,
		ErrLogs =			0x1200000,
		ErrQExecutor =			0x1400000,
		ErrQParser =			0x1800000,
		ErrClient =			0x2000000,
		ErrServer =			0x2200000,
		ErrStore =			0x2400000,
		ErrTManager =			0x2800000,
		ErrTCPProto =			0x4000000,
		ErrIndexes =			0x4200000,
		ErrTypeChecker =		0x4400000,
		ErrUserProgram =		0x4800000,
		ErrProtocol =			0x8200000,
		ErrUnknown =			0x8400000,
		ErrAllModules =			0xFF00000,

// error codes 1-255 (0x01 - 0xFF) reser	ved for system errno.h
		ENoError =			0x0,

		ENoFile =			0x100,
//- Nieobslugiwany/niewlasciwy typ warto	sci w CREATE
		EBadType =			0x200,
//- Blad podczas tworzenia obiektu
		EObjCreate =			0x300,
//- Blad podczas dodawania korzenia
		ERootAdd =			0x400,
//- Nieznany typ wezla drzewa zapytan
		ENoType =			0x500,
//- Proba pobrania (getResult) QueryResu	ltu z pustego zbioru
		EEmptySet =			0x600,
// blad pobierania korzenia
		EGetRoot =			0x700,

// Store
		EBadFile =			0x800,
		EPageNotValid =			0x900,
		EPageNotAquired =		0xA00,

// Parse error
		ENotParsed =			0xB00,
		ENotUniqueNameList =		0xB01,

// Executor
		ENumberExpected =		0xC00,
		ECrcToString =			0xC10,
		ECrcToDouble =			0xC20,
		ECrcToInt =			0xC30,
		ECrcToBool =			0xC40,
		ECrcEltEmptySet =		0xC50,
		ECrcEltMultiple =		0xC60,
		ECrcDelNonOptional =		0xC70,
		ECrcInsExtTooMany =		0xC72,
		EBadInternalCd =		0xC74,
	
		EBoolExpected =			0xD00,
		ERefExpected =			0xE00,
		EBagOfRefExpected =		0xE10,
		EOtherResExp =			0xF00,
		EUnknownValue =			0x1000,
		EUnknownNode =			0x1100,
		EQEmptySet =			0x1200,
		EDivBy0 =			0x1300,
		EQEUnexpectedErr =		0x1400,
		ETransactionOpened =		0x1410,
		EEnvStackCorrupt =		0x1410,
		EEvalStopped =			0x1500,
		EProcNotSingle =		0x1600,
		EBadViewDef =			0x1700,
		EBadBindName =			0x1800,
		EOperNotDefined =		0x1900,
		ENotUniqueClassName =		0x1910,
		ENoClassDefFound =		0x1920,
		EOneResultExpected =		0x1930,
		ENotUniqueInterfaceName =	0x1940,
		ENoInterfaceFound =		0x1950,
		ENoImplementationFound =	0x1960,
		EObjectInsteadOfType =		0x1970,
		EMdnCreateError =		0x1980,
		ERecurrentTypes =		0x1990,
		ESuchMdnExists =		0x1991,
		ECrcCrtExtTooMany =		0x1992,
	
// Config
		ENotInit =			0x1A00,
		ENoValue =			0x1B00,
		EBadValue =			0x1C00,

// Server
		EReceive =			0x1D00,
		EParse =			0x1E00,
		EExecute =			0x1F00,
		ESerialize =			0x2000,
		ESend =				0x2100,
		EBadResult =			0x2200,
		EClientLost =			0x2300,
		EProtocol =			0x2320,

// Transaction
		EDeadlock =			0x2400,
		ESemaphoreInit =		0x2500,
		EUpgradeLock =			0x2600,
		EMutexInit =			0x2700,

// plug for h_errno from gethost	byname(3)
		ENoHost =			0x2800,
		EUnknownPackage	=		0x2900,

// Indexes
		EIndexExists =			0x2A00,
		EFieldIndexed =			0x2B00,
		ENoIndex =			0x2C00,
		ENoIndexedField	=		0x2D00,
		EIndexedFieldDuplicated =	0x2E00,
		ERemoveIndexed =		0x2F00,
		EMetaIncorrect =		0x3000,
		EIncorrectState =		0x3100,
		ENoObject =			0x3200,

// Type Checker
		ECannotRestore =		0x3300,
		ECannotRestoreBadName =		0x3310,
		ENameNotBound =			0x3320,
		EGeneralTCError =		0x3330,
		ETCNotApplicable =		0x3340,
		EIncompleteMetadata =		0x3350,
		EMetadataOutdated =		0x3360,
		ETCInnerFailure =		0x3370,
		ETCInnerRuleUnknown =		0x3371,
		ETCInnerNULLFailure =		0x3372,
		ESigTypesDiffer =		0x3373,
		ESigCdOverflow =		0x3374,
		ESigMissedSubs =		0x3375,
		ESigCdDynamic =			0x3376,
		ESigTNamesDiffer =		0x3377,


// stopper
		EUnknown =			0x3400,

// Logs
		EBadLSN =			0x3500,
		ESyncLog =			0x3510,

// User Program Errors
		EUserUnknown =			0x8000,
		EUserWrongParam =		0x8100
	};
	string SBQLstrerror(int error);
	const string &err_module_desc(int error);
}

#endif
