#include <string>
#include <Errors/Errors.h>
#include <string.h>

using namespace std;

namespace Errors {

	const struct ErrorMessages {
		int e;
		string s;
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
		{ECrcToString,		"Coerce failed: Cannot coerce to String"},
		{ECrcToDouble,		"Coerce failed: Cannot coerce to Double"},
		{ECrcToInt,			"Coerce failed: Cannot coerce to Integer"},
		{ECrcToBool,		"Coerce failed: Cannot coerce to Bool"},
		{ECrcEltEmptySet, 	"Coerce element() failed: Set is empty"},
		{ECrcEltMultiple, 	"Coerce element() failed: Set has more than 1 element"}, 
		{ECrcDelNonOptional,"Coerce delete check failed: Cannot delete non-optional elements"},
		{ECrcCrtExtTooMany, "Coerce create check failed: Cannot create more objects than declared."},
		{ECrcInsExtTooMany, "Coerce insert check failed: Inserting more subobjects than declaration allows."},
		{EBadInternalCd, 	"Coerce dynamic check failed: Cards of subobjects do not match."},
		{EBoolExpected,    "BoolResult expected"},
		{ERefExpected,     "RefResult expected"},
		{EOtherResExp,     "Other Result Type expected"},
		{EUnknownValue,    "Unknown Value Type"},
		{EUnknownNode,     "Unknown Tree Node"},
		{EQEmptySet,       "Empty Set"},
		{EDivBy0,          "Division by 0"},
		{EQEUnexpectedErr, "Unexpected QE Error"},
		{ETransactionOpened, "Transaction already opened"},
		{EEnvStackCorrupt, "Environment Stack was Corrupted"},
		{EEvalStopped,     "Query evaluation stopped by Server"},
		{EProcNotSingle,   "Not one Procedure with this signature"},
		{EBadViewDef,      "Error in View definition"},
		{EBadBindName,     "Error in Name binding"},
		{EOperNotDefined,  "This operation is not defined for this object"},
		{ESuchMdnExists,   "Metadata entry with such name already exists"},
		{EObjectInsteadOfType, "Object declared based on other object, instead of defined type"}, 
		{EMdnCreateError, "Error while performing DB inserts on Metadata nodes"},
		{ERecurrentTypes,  "Cannot define recurrent or mutually recurrent types"},
		
		{ENoInterfaceFound, "Interface not found"},
		{ENoImplementationFound, "Implementation not found"},
		{ENotUniqueInterfaceName,	"Interface name is not unique"},
		{ENotUniqueInterfaceObjectName, "Interface object name is not unique"},
		{ENoSchemaFound, "Schema not found"},
		{ENotUniqueSchemaName,  "Schema name is not unique"},
		{EUserHasNoSchema,  "User has no schema granted to access db"},
		{EUserHasInvalidSchema, "Schema for user is invalid"},
		{ESpoilsInterfaceHierarchy, "Adding or removing interface spoils hierarchy validity"},
		{ESchemaIsCriticallyInvalid, "Schema is critically invalid (ie. defined interfaces missing)"},
		{ECannotAddSchema, "Cannot add schema"},
		{ESchemaInvalidCannotExport, "Schema is invalid and cannot be exported"},
		{ESchemaAlreadyExists, "Schema already exists"},
		{EInterfaceHierarchyInvalid, "Interface hierarchy is invalid"},
		{ECannotBindNoObjectName, "Cannot bind interface: it has no object name defined"},
		{EInterfaceMapCorrupted, "Interface map corrupted"},
		{EClassGraphCorrupted, "Class Graph corrupted"},
		{EInvalidSchemaDefinition, "Invalid schema definition"},
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
		{EDeadlock,        "Deadlock prevention caused transaction abort"},
		{ESemaphoreInit,   "ESemaphoreInit"},
		{EUpgradeLock,     "EUpgradeLock"},
		{EMutexInit,       "EMutexInit"},
		// plug for h_errno from gethostbyname(3)
		{ENoHost,          "Host not found"},
		{EUnknownPackage,  "Unknown Package Type"},
		// Indexes
		{EIndexExists,				"Index already exists"},
		{EFieldIndexed,				"Field is already indexed"},
		{ENoIndex,					"No such index"},
		{ENoIndexedField,			"Indexed field not found"},
		{EIndexedFieldDuplicated,	"Indexed field already exists"},
		{ERemoveIndexed,			"Indexed field cannot be removed"},
		{EMetaIncorrect,			"Incorrect index metadata"},
		{EIncorrectState,			"Incorrect object state"},
		{ENoObject,					"Object not exists"},
		// TypeChecker
		{ECannotRestore, 	"Unable to restore type checking process"},
		{ECannotRestoreBadName, "Unable to restore type checking after Bad Name error"},
		{EIncompleteMetadata, "Metadata incomplete, unable to typecheck query. "},
		{EMetadataOutdated, "Metadata not up to date, unable to typecheck query. Try: reloadScheme; "},
		{ENameNotBound, "Name could not be bound"},
		{EGeneralTCError, "TypeCheck general error - query invalid"},
		{ETCNotApplicable, "TypeChecking not applicable to query - unknown operators included."},
		{ETCInnerFailure, "TypeCheck inner failure."},
		{ETCInnerRuleUnknown, "TypeCheck error caused by unknown rule type."},
		{ETCInnerNULLFailure, "TypeCheck error caused by a NULL pointer, non-NULL expected."},
		{ESigTypesDiffer, "Error while comparing signatures: miss-matched types."},
		{ESigCdOverflow, "Too many objects/subobjects as compared to cards declared."},
		{ESigMissedSubs, " Non-optional fields missing."},
		{ESigCdDynamic, "TC dynamic coertion: checking cardinalities in runtime."},
		{ESigTNamesDiffer, "Error while comparing signatures: distinct typenames don't match."},

		// UserProgram
		{EUserUnknown, "Exception Thrown in Query Execution - Unknown User Error"},
		{EUserWrongParam, "Exception Thrown in Query Execution - Wrong Parameter"},

		// Logs
		{EBadLSN, "Log entry couldn't be found."},
		{ESyncLog, "Error while synchronising the log file."},

		// THIS MUST BE THE LAST ENTRY
		{EUnknown,         "Unknown error"}
	}, 
	ErrorModules[] = 
	{
		{ErrBackup, "Backup"},
		{ErrConfig, "Config"},
		{ErrErrors, "Errors"},
		{ErrLockMgr, "Lock Manager"},
		{ErrLogs, "Logs"},
		{ErrQExecutor, "Query Executor"},
		{ErrQParser, "Query Parser"},
		{ErrClient, "Client"},
		{ErrServer, "Server"},
		{ErrStore, "Store"},
		{ErrTManager, "Transaction Manager"},
		{ErrIndexes, "Indexes"},
		{ErrTypeChecker, "Type Checker"},
		{ErrUserProgram, "User Program"},
		{ErrProtocol, "Protocol"},
		/* should be the last one */
		{ErrUnknown, "Unknown"}
	};

	const string &err_module_desc(int error){
		int eidx = error & ErrAllModules;
		int i;
		for (i = 0; ErrorModules[i].e != ErrUnknown && ErrorModules[i].e
				!= eidx; ++i) {}
		return ErrorModules[i].s;
	}
	string SBQLstrerror(int error)
	{
		const string &module(err_module_desc(error));
		if ((error & ~ErrAllModules) < 0x100) {
			return module + ": " + ::strerror(error & ~ErrAllModules);
		} else {
			int erridx = (error & ~ErrAllModules);
			int i = 0;
			while (ErrorMsg[i].e != EUnknown) {
				if (ErrorMsg[i].e == erridx)
					break;
				i++;
			}
			return module + ": " + ErrorMsg[i].s;
		}
	}
}

