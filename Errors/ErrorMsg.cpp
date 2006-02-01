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
		"Division by 0"
		"Unexpected QE Error"
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
		"Unknown error"
	};
}
