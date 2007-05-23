package pl.tzr.browser.store.node;

import java.util.Collection;

import pl.tzr.driver.loxim.exception.SBQLException;

/**
 * Pointer to the data object in LoXiM database
 * Allows to fetch and modify contents of the object
 *  
 * @author Tomasz Rosiek
 *
 */
public interface Node {
	
	/**
	 * Returns the reference to the object represented by this node
	 * @return
	 */
	String getReference();
	
	/**
	 * Returns the name of the object represented by this node
	 * @return
	 */
	String getName();
	
	/**
	 * Returns the value of the object represented by this node
	 * TODO Should throw an exception if object is composite
	 * @return
	 * @throws SBQLException
	 */
	ObjectValue getValue() throws SBQLException;
	
	/**
	 * Sets a value of the simple object
	 * TODO Should trow an exception if object is composite
	 * @param value
	 * @throws SBQLException
	 */
	void setValue(ObjectValue value) throws SBQLException;
	
	/**
	 * Gets child nodes of the object if object is complex
	 * TODO Should throw an axception if object is not complex
	 * @param propertyName
	 * @return
	 * @throws SBQLException
	 */
	Collection<Node> getChildNodes(String propertyName) throws SBQLException;
	
	/**
	 * Gets the unique child node of the object. Throws an exception
	 * if object has more than one child nodes, returns null if object
	 * doesn't contain any child nodes 
	 * @param propertyName
	 * @return
	 * @throws SBQLException
	 */
	Node getUniqueChildNode(String propertyName) throws SBQLException;
	
	/**
	 * Adds new node to the object
	 * TODO Throw an exception if object is simple
	 * @param child
	 * @throws SBQLException
	 */
	void addChild(Node child) throws SBQLException;
	
	/**
	 * Deletes the object represented by the node
	 * @throws SBQLException
	 */
	void delete() throws SBQLException;
	
}
