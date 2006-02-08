#include <string>
#include "Errors.h"

using namespace std;

namespace Errors {
	char *ErrorMsg[] = {
		"No error",
		"No such file",
		"Not supported or incorrect type in CREATE",
		"Error creating object",
		"Error adding root",
		"Unknown query tree node type",
		"Set empty",
		"Error geting root",
	// Store
		"Incorrect file",
		"Page not valid",
		"Page not aquired",
	// Parse error
		"Parse error",
	// Executor
		"NumberResult expected",
		"BoolResult expected",
		"RefResult expected",
		"Some Other Result Type expected",
		"Unknown Value Type",
		"Unknown Tree Node",
		"Empty Set",
		"Division by 0",
		"Unexpected QE Error",
	// Config
		"ENotInit",
		"ENoValue",
		"EBadValue",
	// Server
		"EReceive",
		"EParse",
		"EExecute",
		"ESerialize",
		"ESend",
		"EBadResult",
		"EClientLost",
	// Transaction
		"EDeadlock",
		"ESemaphoreInit",
		"EUpgradeLock",
		"EMutexInit",
	// plug for h_errno from gethostbyname(3)
		"Host not found",
		"Unknown error",
	};

	string *SBQLstrerror(int error)
	{
		string src_mod, dest_mod, str;

		switch(error & ErrAllModules) {
			case ErrBackup:
				src_mod = "Backup";
				break;
			case ErrConfig:
				src_mod = "Config";
				break;
			case ErrDriver:
				src_mod = "Driver";
				break;
			case ErrErrors:
				src_mod = "Errors";
				break;
			case ErrLockMgr:
				src_mod = "Lock Manager";
				break;
			case ErrLogs:
				src_mod = "Logs";
				break;
			case ErrQExecutor:
				src_mod = "Query Executor";
				break;
			case ErrQParser:
				src_mod = "Query Parser";
				break;
			case ErrSBQLCli:
				src_mod = "SBQLCli";
				break;
			case ErrServer:
				src_mod = "Server";
				break;
			case ErrStore:
				src_mod = "Store";
				break;
			case ErrTManager:
				src_mod = "Transaction Manager";
				break;
			default:
				src_mod = "Unknown";
				break;
		}
		if ((error & ~ErrAllModules) < 0x100) {
			str = strerror(error & ~ErrAllModules);
		} else {
			int erridx = (error & ~ErrAllModules);
			erridx = erridx < EUnknown ? erridx / 0x100 : EUnknown / 0x100;
			str = ErrorMsg[erridx];
		}

		return new string(src_mod + ": " + str);
	};
}
