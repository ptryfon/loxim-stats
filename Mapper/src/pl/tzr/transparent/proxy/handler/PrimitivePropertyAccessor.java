package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.transparent.TransparentProxyFactory;
import pl.tzr.transparent.structure.model.PropertyInfo;

public abstract class PrimitivePropertyAccessor<T> implements PropertyAccessor<T> {
	
	public T retrieveFromBase(
			Node parent, 
			String propertyName, 
			PropertyInfo info, 
			TransparentProxyFactory transparentProxyFactory) 
		throws InvalidDataStructure, SBQLException {
		
		ObjectValue value = parent.getValue();
		if (!(value instanceof ComplexValue)) throw new InvalidDataStructure();
		
		Node foundNode = parent.getUniqueChildNode(propertyName);										
		T result = fetchPrimitiveValue(foundNode);
		
		return result;		
			
 
	}
	
	public void saveToBase(T data, Node parent, String propertyName, 
			PropertyInfo propertyInfo) 
		throws InvalidDataStructure, SBQLException {
		
		ObjectValue value = parent.getValue();
		if (!(value instanceof ComplexValue)) throw new InvalidDataStructure();
		
		Node foundNode = parent.getUniqueChildNode(propertyName);
		
		if (foundNode == null) {
			if (data != null) {
				/* Dodaj nowy */
				throw new UnsupportedOperationException();	
			}
			
		} else {
			if (data != null) {
				/* Zmien istniejacy */
				ObjectValue newValue = createPimitiveValue(data);
				foundNode.setValue(newValue);				
			} else {
				/* Usun istniejacy */
				foundNode.delete();
			}
		}
		
	}	
	
	protected abstract T fetchPrimitiveValue(Node node) throws
		SBQLException, InvalidDataStructure;

	protected abstract ObjectValue createPimitiveValue(T data) throws
		SBQLException, InvalidDataStructure;
	

}
