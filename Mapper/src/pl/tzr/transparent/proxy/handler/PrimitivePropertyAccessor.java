package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.model.PropertyInfo;

public abstract class PrimitivePropertyAccessor<T> implements PropertyAccessor<T> {
	
	public T retrieveFromBase(
			Node parent, 			
			PropertyInfo info, 
			TransparentSession session) 
		throws SBQLException, DeletedException {
		
		ObjectValue value = parent.getValue();
		if (!(value instanceof ComplexValue)) 
			throw new InvalidDataStructureException();
		
		Node foundNode = parent.getUniqueChildNode(info.getNodeName());
		if (foundNode == null) {
			return null;
		} else {
		
			T result = fetchPrimitiveValue(foundNode, info, session);		
			return result;
			
		}
 
	}
	
	public void saveToBase(T data, 
			Node parent, 
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws SBQLException, DeletedException {
		
		ObjectValue value = parent.getValue();
		if (!(value instanceof ComplexValue)) throw new InvalidDataStructureException();
		
		Node foundNode = parent.getUniqueChildNode(propertyInfo.getNodeName());
		
		if (foundNode == null) {
			if (data != null) {
				/* Create new node */
				ObjectValue newValue = createPrimitiveValue(data, propertyInfo, session);
				Node newNode = new LoximNode(propertyInfo.getNodeName(), newValue);
				parent.addChild(newNode);
			}
			
		} else {
			if (data != null) {
				/* Change existing node */
				ObjectValue newValue = createPrimitiveValue(data, propertyInfo, session);
				foundNode.setValue(newValue);				
			} else {
				/* Remove existing node */
				foundNode.delete();
			}
		}
		
	}	
	
	protected abstract T fetchPrimitiveValue(
			Node node, 
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws SBQLException, InvalidDataStructureException;

	protected abstract ObjectValue createPrimitiveValue(
			T data, 
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws SBQLException, InvalidDataStructureException;
	

}
