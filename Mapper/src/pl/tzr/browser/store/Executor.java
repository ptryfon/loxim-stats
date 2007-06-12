package pl.tzr.browser.store;

import java.util.Collection;
import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;

/**
 * Concrete classes of this interface provide implementations of 
 * all needed operations on the database
 * 
 * @author Tomasz Rosiek
 *
 */
public interface Executor {
	
	/**
	 * Executes query and returns result nodes
	 * @param query
	 * 	 query
	 * @return
	 * @throws SBQLException
	 */
	Set<Node> executeQuery(String query) throws SBQLException;
	
	/**
	 * Returns object's child nodes of provided name
	 * @param parentRef
	 * 		reference to parent object
	 * @param propertyName
	 * 		name of the child node
	 * @return
	 * 		collection of the child noded of provided name
	 * @throws SBQLException
	 */
	Collection<Node> getChildNodes(String parentRef, String propertyName) 
		throws SBQLException;
	
	/**
	 * Returns value of a object defined by a reference
	 * @param ref
	 * 		reference of object to fetch
	 * @return
	 * 		object, null if object with this reference doesn't exist
	 * @throws SBQLException
	 */
	ObjectValue getValue(String ref) throws SBQLException, DeletedException;
	
	/**
	 * Sets the vaule of a object
	 * @param ref
	 * @param value
	 * @throws SBQLException
	 */
	void setValue(
			String ref, 
			ObjectValue value
		) throws SBQLException;
	
	void addChild(String parentRef, Node child) 
		throws SBQLException, DeletedException;
	
	Node createSimpleObject(String name, ObjectValue value) 
		throws SBQLException, DeletedException;
	
	void deleteObject(String ref) throws SBQLException;
		
}
