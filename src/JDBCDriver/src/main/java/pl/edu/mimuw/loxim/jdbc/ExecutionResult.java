package pl.edu.mimuw.loxim.jdbc;

import java.util.List;

class ExecutionResult {

	private List<Object> result;
	private int updates;
	
	public List<Object> getResult() {
		return result;
	}
	
	public void setResult(List<Object> result) {
		this.result = result;
	}
	
	public int getUpdates() {
		return updates;
	}
	
	public void setUpdates(int updates) {
		this.updates = updates;
	}
	
}
