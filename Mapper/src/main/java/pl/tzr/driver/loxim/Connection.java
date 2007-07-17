package pl.tzr.driver.loxim;

import java.util.Map;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.result.Result;

/**
 * Interface of connection with semistructural database
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public interface Connection {
	
	/**
	 * Excecutes simple query
	 * @param query query text
	 * @return query result
	 * @throws SBQLException
	 */
	public Result execute(String query) throws SBQLException;
	
	/**
	 * Prepares parametrized query
	 * @param query query text. Parameters should be named and look like $arg
	 * @return identifier of parametrized query, to be used in 
	 * execute(statementId, params) method
	 * @throws SBQLException
	 */
	public long parse(String query) throws SBQLException;
	
	/**
	 * Executes parametrized query
	 * @param statementId identifier of parametrized query
	 * @param params map of parameter values
	 * @return query result
	 * @throws SBQLException
	 */
	public Result execute(long statementId, Map<String, Result> params) throws SBQLException;
		
	/**
	 * Closes connection with database
	 * @throws SBQLException
	 */
	public void close() throws SBQLException;
}