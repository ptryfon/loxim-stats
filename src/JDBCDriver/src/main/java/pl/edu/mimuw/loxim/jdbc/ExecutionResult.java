package pl.edu.mimuw.loxim.jdbc;

import java.sql.SQLException;
import java.util.List;

class ExecutionResult {

	private final int updates;
	private final List<Object> result;
	private LoXiMStatement stmt;
		
	public ExecutionResult(List<Object> result, int updates) throws SQLException {
		this.updates = updates;
		this.result = result;
	}
	
	public void setStmt(LoXiMStatement stmt) {
		if (this.stmt != null) {
			throw new IllegalStateException("Statement already set");
		}
		this.stmt = stmt;
	}
	
	public int getUpdates() {
		return updates;
	}
		
	public LoXiMResultSet asLoXiMResultSet() throws SQLException {
		if (stmt == null) {
			throw new IllegalStateException("Statement not set");
		}
		return new LoXiMResultSetImpl(stmt, result);
	}
	
	public boolean isEmpty() {
		return result.isEmpty();
	}
}
