package pl.tzr.driver.loxim;

import java.util.Map;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.result.Result;

/**
 * Interface for the connection introducing some useful higher level operations
 * like authentication and transaction support.
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public interface SimpleConnection extends Connection {

	/**
	 * Prepares and executes parametrized query
	 * @param query query text
	 * @param params map of query parameters
	 * @return query results
	 * @throws SBQLException
	 */
	Result executeNamed(String query, Map<String, Result> params)
			throws SBQLException;

	/**
	 * Parses and executes parametrized query with non-named parameters (parameters
	 * are provided as variable amount of method arguments)
	 * @param query query text. Parameter placeholders should be marked with ?  
	 * @param params query parameters
	 * @return query results
	 * @throws SBQLException
	 */
	Result executeParam(String query, Result... params) throws SBQLException;
	
	/**
	 * Starts the transaction
	 * @throws SBQLException
	 */
	void beginTransaction() throws SBQLException;

	/**
	 * Commits the transaction
	 * @throws SBQLException
	 */
	void commitTransation() throws SBQLException;

	/**
	 * Rollbacks the transaction
	 * @throws SBQLException
	 */
	void rollbackTransaction() throws SBQLException;

}