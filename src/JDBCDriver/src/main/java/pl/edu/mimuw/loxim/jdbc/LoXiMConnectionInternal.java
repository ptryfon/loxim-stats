package pl.edu.mimuw.loxim.jdbc;

import java.sql.SQLException;

interface LoXiMConnectionInternal extends LoXiMConnection {

	public ExecutionResult execute(LoXiMStatement stmt, String sbql) throws SQLException;

	public abstract void cancel() throws SQLException;
	
}
