package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.transparent.TransparentProxyFactory;
import pl.tzr.transparent.structure.model.PropertyInfo;

public interface PropertyAccessor<T> {

	T retrieveFromBase(
			Node parent, String propertyName, PropertyInfo propertyInfo,
			TransparentProxyFactory transparentProxyFactory) 
		throws InvalidDataStructure, SBQLException;
	
	void saveToBase(T data, Node parent, String propertyName, 
			PropertyInfo propertyInfo) 
		throws InvalidDataStructure, SBQLException;
	
}
