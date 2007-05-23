package pl.tzr.browser.session;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;

/**
 * Single working session with the database. One session lasts one transaction,
 * so for every session the new transaction is created. Session is 
 * responsible for object caching and transaction handling.
 * Transparent object are valid only during single session.
 * @author Tomasz Rosiek
 *
 */
public interface Session {

	/**
	 * Executes the query on the database and returns a list of nodes
	 * with query results.
	 * @param query
	 * @return
	 * @throws SBQLException
	 */
	Set<Node> find(String query) throws SBQLException;
	
	/**
	 * Commits and closes the session
	 */	
	void commit();

	/**
	 * Commits and rollbacks the session
	 */		
	void rollback();

	/**
	 * Returns true if the session is active
	 */
	boolean isActive();

}