package pl.tzr.transparent.proxy.handler;


import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class ComponentPropertyAccessor implements PropertyAccessor {
		
	public Object retrieveFromBase(
			Node parent, 
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws SBQLException, DeletedException {
		
		Class desiredClass = propertyInfo.getClazz();
		
		ObjectValue value = parent.getValue();
		
		if (!(value instanceof ComplexValue)) 
			throw new InvalidDataStructureException();
		
		Node foundProxy = parent.getUniqueChildNode(propertyInfo.getPropertyName());
		
		if (foundProxy == null) return null;
		
		Object result = session.getDatabaseContext().
			getTransparentProxyFactory().
			createProxy(foundProxy, desiredClass, session);
		
		return result;
		
	}
	
	public void saveToBase(
			Object data, 
			Node parent, 
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws SBQLException, DeletedException {
		
		ObjectValue value = parent.getValue();
		if (!(value instanceof ComplexValue)) throw new InvalidDataStructureException();
		
		Node foundNode = parent.getUniqueChildNode(propertyInfo.getPropertyName());
		
		if (foundNode == null) {
			if (data != null) {
				/* Create new node */				
				Node newNode = session.getDatabaseContext().
					getModelRegistry().
					createNodeRepresentation(data, propertyInfo.getPropertyName(), session);
				parent.addChild(newNode);
			}
			
		} else {
			if (data != null) {
				/* Delete existing node and create new one */
				foundNode.delete();
				Node newNode = session.getDatabaseContext().
					getModelRegistry().
					createNodeRepresentation(data, propertyInfo.getPropertyName(), session);
				parent.addChild(newNode);				
			} else {
				/* Remove existing node */
				foundNode.delete();
			}
		}		
		
	}		
	
}
