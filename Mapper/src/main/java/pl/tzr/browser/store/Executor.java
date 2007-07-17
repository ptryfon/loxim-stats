package pl.tzr.browser.store;

import java.util.Collection;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.exception.SBQLException;

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
	Collection<Node> find(String query, ObjectValue... params) 
        throws SBQLException;
    
    /**
     * Extecutes query and returns collection of primitive values
     * @param query
     * @param objectValues
     * @return
     * @throws SBQLException
     */
    Collection<SimpleValue> findPrimitive(
            String query, 
            ObjectValue... objectValues) 
        throws SBQLException;     
	
	/**
	 * Returns object's child nodes of provided name
	 * @param parentRef
	 * 		reference to parent object
	 * @param nodeName
	 * 		name of the child node
	 * @return
	 * 		collection of the child noded of provided name
	 * @throws SBQLException
	 */
	Collection<Node> getChildNodes(String parentRef, String propertyName);
	
	/**
	 * Returns value of a object defined by a reference
	 * @param ref
	 * 		reference of object to fetch
	 * @return
	 * 		object, null if object with this reference doesn't exist
	 * @throws SBQLException
	 */
	ObjectValue getValue(String ref);
	
	/**
	 * Sets the vaule of a object
	 * @param ref
	 * @param value
	 * @throws SBQLException
	 */
	void setValue(String ref, ObjectValue value);
	
	void addChild(String parentRef, Node child);
	
	Node createSimpleObject(String name, ObjectValue value);
	
	void deleteObject(String ref);
		
}
