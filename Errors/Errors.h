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

#define ENoFile			    0x1
}

#endif
