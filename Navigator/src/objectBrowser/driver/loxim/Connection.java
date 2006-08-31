package objectBrowser.driver.loxim;

import objectBrowser.driver.loxim.result.Result;


public interface Connection {
	
	public Result execute(String query) throws SBQLException;
	public Result getObjectByRef(String ref) throws SBQLException;
	public void close() throws SBQLException;
}