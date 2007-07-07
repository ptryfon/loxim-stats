package pl.tzr.browser.session;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;

/**
 * Single working session with the database. One session lasts one transaction,
 * so for every session the new transaction is created. Session is 
 * responsible for object caching and transaction handling.
 * Node objects are valid only during single session.
 * @author Tomasz Rosiek
 *
 */
public interface Session {

	/**
	 * Executes the query on the database and returns a list of nodes
	 * with query results.
	 * @param query
     * @param params - query parameters
	 * @return
	 * @throws SBQLException
	 */
	Set<Node> find(String query, ObjectValue... params) throws SBQLException;
	
	/**
	 * Persists the node with it's subnodes and puts it into the root
	 * @param node
	 * 		node to be persisted. Have to be detached
	 * @throws SBQLException
	 */
	void addToRoot(Node node) throws SBQLException, DeletedException;
	
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