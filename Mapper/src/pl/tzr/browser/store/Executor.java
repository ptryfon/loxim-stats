package pl.tzr.browser.store;

import java.util.Collection;
import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.driver.loxim.exception.SBQLException;

/**
 * Concrete classes of this interface provide implementations of 
 * all needed operations on the database
 * 
 * @author Tomasz Rosiek
 *
 */
public interface Executor {
	
	Set<Node> executeQuery(String query) throws SBQLException;
	
	Collection<Node> getChildNodes(String parentRef, String propertyName) throws SBQLException;
	
	ObjectValue getValue(String ref) throws SBQLException;
	
	void setValue(String ref, ObjectValue value) throws SBQLException;
	
	void addChild(String parentRef, Node child) throws SBQLException;
	
	Node createObject(String name, ObjectValue value) throws SBQLException;
	
	void deleteObject(String ref) throws SBQLException;
		
}
