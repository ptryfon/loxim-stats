#include <string>
#include "Errors.h"

using namespace std;

namespace Errors {

struct {
	int e;
	char *s;
} ErrorMsg[] = {
	{ENoError,         "No error"},
	{ENoFile,          "No such file"},
	{EBadType,         "Not supported or incorrect type in CREATE"},
	{EObjCreate,       "Error creating object"},
	{ERootAdd,         "Error adding root"},
	{ENoType,          "Unknown query tree node type"},
	{EEmptySet,        "Set empty"},
	{EGetRoot,         "Error geting root"},
// Store
	{EBadFile,         "Incorrect file"},
	{EPageNotValid,    "Page not valid"},
	{EPageNotAquired,  "Page not aquired"},
// Parse error
	{ENotParsed,       "Parse error"},
// Executor
	{ENumberExpected,  "NumberResult expected"},
	{EBoolExpected,    "BoolResult expected"},
	{ERefExpected,     "RefResult expected"},
	{EOtherResExp,     "Some Other Result Type expected"},
	{EUnknownValue,    "Unknown Value Type"},
	{EUnknownNode,     "Unknown Tree Node"},
	{EQEmptySet,       "Empty Set"},
	{EDivBy0	,  "Division by 0"},
	{EQEUnexpectedErr, "Unexpected QE Error"},
// Config
	{ENotInit,         "ENotInit"},
	{ENoValue,         "ENoValue"},
	{EBadValue,        "EBadValue"},
// Server
	{EReceive,         "EReceive"},
	{EParse,           "EParse"},
	{EExecute,         "EExecute"},
	{ESerialize,       "ESerialize"},
	{ESend,            "ESend"},
	{EBadResult,       "EBadResult"},
	{EClientLost,      "EClientLost"},
// Transaction
	{EDeadlock,        "EDeadlock"},
	{ESemaphoreInit,   "ESemaphoreInit"},
	{EUpgradeLock,     "EUpgradeLock"},
	{EMutexInit,       "EMutexInit"},
// plug for h_errno from gethostbyname(3)
	{ENoHost,          "Host not found"},

// THIS MUST BE THE LAST ENTRY
	{EUnknown,         "Unknown error"}
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
			int i = 0;
			while (ErrorMsg[i].e != EUnknown) {
				if (ErrorMsg[i].e == erridx)
					break;
				i++;
			}
			str = ErrorMsg[i].s;
		}

		return new string(src_mod + ": " + str);
	};
}
