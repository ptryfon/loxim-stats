package pl.tzr.transparent;

import java.util.Set;

import pl.tzr.driver.loxim.exception.SBQLException;

public interface TransparentSession {
		
	/**
	 * Executes the query on the database and return a list of results
	 * as proxied objects. 
	 * Query should return the list of objects mapped in the data model
	 * definition (@see desiredClass parameter)
	 * @param query query in SBQL language
	 * @param desiredClass class of the objects wich will be returned as the
	 * results of the query. Query should return nodes possible to be mapped
	 * to desiredClass, according to the used data model definition.
	 * @throws SBQLException
	 */
	Set<Object> find(String query, Class desiredClass) throws SBQLException;
	
	/**
	 * Persists the object in the database
	 * @param object to be persisted
	 */
	Object persist(Object object);
	
	/**
	 * Removes object representation from the database. After execution of this
	 * method the object will become transient
	 * @param object to be deleted
	 */
	void delete(Object object);
	
	/**
	 * Returns true when the session is active
	 * @return
	 */
	boolean isActive();
	
	/**
	 * Commits and closes the session
	 */
	void commit();
	
	/**
	 * Rollbacks and closes the session
	 */
	void rollback();
	
	DatabaseContext getDatabaseContext();
}
