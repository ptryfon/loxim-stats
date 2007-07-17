package pl.tzr.browser.session;

import java.util.Collection;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.exception.SBQLException;

/**
 * Single working session with the database. One session lasts one transaction,
 * so for every session the new transaction is created. Session is responsible
 * for object caching and transaction handling. Node objects are valid only
 * during single session.
 * @author Tomasz Rosiek
 */
public interface Session {

    /**
     * Executes the query on the database and returns a list of nodes with query
     * results.
     * @param query
     *            query text with question marks as parameter placeholders
     * @param params
     *            query parameters
     * @return query results
     * @throws SBQLException
     *             when database exception occurs
     */
    Collection<Node> find(String query, ObjectValue... params)
            throws SBQLException;

    /**
     * Executes the query that should return primitive values on the database
     * and returns a list of primitives.
     * @param query
     *            query text with question marks as parameter placeholders
     * @param params
     *            query parameters
     * @return query results
     * @throws SBQLException
     *             when database exception occurs
     */
    Collection<SimpleValue> findPrimitive(String query, ObjectValue... params)
            throws SBQLException;

    /**
     * Persists the node with it's subnodes and puts it into the root.
     * 
     * @param node
     *            node to be persisted. Have to be detached
     * @throws SBQLException
     *             when database exception occurs
     */
    void addToRoot(Node node) throws SBQLException;
    
    /**
     * Returns the node with specified reference
     * @param ref reference of the node to fetch
     * @return node with specified reference, null if doesn't exist
     */
    Node fetchNode(final String ref);

    /**
     * Commits and closes the session.
     */
    void commit();

    /**
     * Commits and rollbacks the session.
     */
    void rollback();

    /**
     * Returns true if the session is active.
     * 
     * @return true if the session is active
     */
    boolean isActive();

}
