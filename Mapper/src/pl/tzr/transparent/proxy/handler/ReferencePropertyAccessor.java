package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.transparent.TransparentProxyFactory;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class ReferencePropertyAccessor implements PropertyAccessor {

	public Object retrieveFromBase(Node parent, String propertyName,
			PropertyInfo propertyInfo,
			TransparentProxyFactory transparentProxyFactory)
			throws InvalidDataStructure, SBQLException {
		
		Class desiredClass = propertyInfo.getClazz();
		
		ObjectValue value = parent.getValue();
		if (!(value instanceof ComplexValue)) throw new InvalidDataStructure();
		
		Node foundNode = parent.getUniqueChildNode(propertyName);
		
		if (foundNode == null) return null;
		
		if (!(foundNode.getValue() instanceof ReferenceValue)) 
			throw new InvalidDataStructure();
		
		Node targetNode = ((ReferenceValue)foundNode.getValue()).getTargetNode(); 
		
		Object result = transparentProxyFactory.createProxy(targetNode, desiredClass);
		
		return result;
		
	}
	
	public void saveToBase(Object data, Node parent, String propertyName, 
			PropertyInfo propertyInfo) throws InvalidDataStructure, SBQLException {
		
		throw new UnsupportedOperationException();
		
	}
	
}
