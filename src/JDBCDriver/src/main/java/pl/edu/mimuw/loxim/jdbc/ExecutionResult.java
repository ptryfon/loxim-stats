package pl.edu.mimuw.loxim.jdbc;

import java.sql.SQLException;
import java.util.List;

class ExecutionResult {

	private final int updates;
	private final int size;
	private final LoXiMResultSet resultSet;
		
	public ExecutionResult(LoXiMStatement stmt, List<Object> result, int updates) throws SQLException {
		this.updates = updates;
		this.size = result.size();
		resultSet = new LoXiMResultSetImpl(stmt, result);
	}
	
	public int getUpdates() {
		return updates;
	}
		
	public LoXiMResultSet asLoXiMResultSet() throws SQLException {
		return resultSet;
	}
	
	public boolean isEmpty() {
		return size == 0;
	}
}
