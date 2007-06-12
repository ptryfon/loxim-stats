package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.model.PropertyInfo;

public interface PropertyAccessor<T> {

	T retrieveFromBase(Node parent, PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws SBQLException, DeletedException;
	
	void saveToBase(T data, Node parent, PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws SBQLException, DeletedException;
	
}
