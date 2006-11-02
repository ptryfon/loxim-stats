package objectBrowser.driver.loxim;

import objectBrowser.driver.loxim.result.Result;


public interface Connection {
	
	/**
	 * Wykonanie prostego zapytania
	 * @param query
	 * @return
	 * @throws SBQLException
	 */
	public Result execute(String query) throws SBQLException;
	
	/**
	 * Sparsowanie zapytania sparametryzowanego
	 * @param query
	 * @return
	 * @throws SBQLException
	 */
	public Statement parse(String query) throws SBQLException;
	
	/**
	 * Wykonanie zapytania sparametryzowanego
	 * @param statement
	 * @return
	 * @throws SBQLException
	 */
	public Result execute(Statement statement) throws SBQLException;
	
	public Result getObjectByRef(String ref) throws SBQLException;
	public void close() throws SBQLException;
}