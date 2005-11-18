#ifndef _ERRORS_H
#define _ERRORS_H

#include <errno.h>

//using namespace std;

namespace Errors {
	// Nazwy bledow:
	// <Namespace z ktorego blad pochodzi><Krotka nazwa>
	// np
	// StoreNoSpace		ENOSPC
#define ErrorClassBackup		 0x1000
#define	ErrorClassConfig		 0x2000
#define	ErrorClassDriver		 0x4000
#define	ErrorClassErrors		 0x8000
#define	ErrorClassLock			0x10000
#define	ErrorClassLog			0x12000
#define	ErrorClassQueryExecutor		0x14000
#define	ErrorClassQueryParser		0x18000
#define	ErrorClassSBQLCli		0x20000
#define	ErrorClassServer		0x22000
#define	ErrorClassStore			0x24000
#define	ErrorClassTransactionManager	0x28000

#define ErrorsNoFile			1 | ErrorClassErrors
}

#endif
