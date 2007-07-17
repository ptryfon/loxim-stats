package pl.tzr.browser.store.node;

import java.util.Collection;

import pl.tzr.browser.session.LoximSession;

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
	 */
	ObjectValue getValue();
	
	/**
	 * Sets a value of the simple object
	 * TODO Should trow an exception if object is composite
	 * @param value
	 */
	void setValue(ObjectValue value);
	
	/**
	 * Gets child nodes of the object if object is complex
	 * TODO Should throw an axception if object is not complex
	 * @param nodeName
	 * @return
	 */
	Collection<Node> getChildNodes(String propertyName);
	
	/**
	 * Gets allchild nodes of the object if object is complex
	 * TODO Should throw an axception if object is not complex
	 * @param nodeName
	 * @return
	 */
	Collection<Node> getAllChildNodes();
	
	/**
	 * Gets the unique child node of the object. Throws an exception
	 * if object has more than one child nodes, returns null if object
	 * doesn't contain any child nodes 
	 * @param nodeName
	 * @return
	 */
	Node getUniqueChildNode(String propertyName);
	
	/**
	 * Adds new node to the object
	 * TODO Throw an exception if object is simple
	 * @param child
	 */
	void addChild(Node child);
	
	/**
	 * Deletes the object represented by the node
	 */
	void delete();	
	
	/**
	 * Returns true if the node is detached from the database, and contains
	 * all the data 
	 * @return
	 * 		true if the node is detached
	 */
	boolean isDetached();
	
	/**
	 * Marks object attached and binds it to current session. This method
	 * should be run when the node is stored 
	 * @param session
	 * 		session to wich the node was attached
	 * @param ref
	 * 		reference of the node in the database
	 */
	void markAttached(LoximSession session, String ref);
	
	/**
	 * Returns true if the childNode is the child of the node represtented
	 * by this object 
	 * @param node
	 */
	boolean isChild(Node childNode);
    
    /**
     * Returns amount of the nodes children containing specified name
     * @param name
     *  name of the child name to count
     * @return amount of the child nodes with specified name
     */
    int childAmount(String name);
	
	/**
	 * Removes all children of the node having specified name
	 * @param childName
	 * 		name of child nodes to remove
	 */
	void removeAllChildren(String childName);
    
    /**
     * Searches for the nodes having a children with provided name and provided
     * value
     * @param name expected name of the children
     * @param value expected value of the children
     * @return collection of nodes fullfiling the criteria
     */
    Collection<Node> hasChildOfValue(String name, ObjectValue value);   

}
