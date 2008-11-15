package pl.edu.mimuw.loxim.jdbc;

import java.sql.SQLException;

interface LoXiMConnectionInternal extends LoXiMConnection {

	public ExecutionResult execute(String stmt) throws SQLException;
	
}
