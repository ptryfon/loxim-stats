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
		"Unknown unOp node",
		"Unknown algOp node",
		"Wrong algOp (arithmetical) argument",
		"Wrong deref argument",
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
	// plug for h_errno from gethostbyname(3)
		"Host not found",
		"Unknown error"
	};
}
