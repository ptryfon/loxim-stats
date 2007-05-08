package pl.tzr.browser.store.node;

import java.util.Collection;

import pl.tzr.driver.loxim.exception.SBQLException;

public interface Node {
	
	String getReference() ;
	
	String getName();
	
	ObjectValue getValue() throws SBQLException;
	
	void setValue(ObjectValue value) throws SBQLException;
	
	Collection<Node> getChildNodes(String propertyName) throws SBQLException;
	
	Node getUniqueChildNode(String propertyName) throws SBQLException;
	
	void addChild(Node child) throws SBQLException;
	
	void delete() throws SBQLException;
}
