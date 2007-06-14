package pl.tzr.driver.loxim;

import pl.tzr.driver.loxim.exception.SBQLException;

/**
 * Interface wich provides connection to the semistructural database.
 * Implementations of this class could create new connections on demand
 * or maintain some pool of existing connections. 
 * 
 * @author Tomasz Rosiek
 *
 */
public interface LoximDatasource {
	
	/**
	 * Gets new connection to the database
	 * @return connection
	 * @throws SBQLException
	 */
	SimpleConnection getConnection() throws SBQLException;
	
	/**
	 * Returns the connection
	 * @param connection connection to be returned
	 * @throws SBQLException
	 */
	void release(SimpleConnection connection) throws SBQLException;

}
