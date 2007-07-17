package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.exception.ObjectNotPersistentException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class ReferencePropertyAccessor extends PrimitivePropertyAccessor {
	
	@Override
    protected Object fetchPrimitiveValue(
			Node node, 
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws InvalidDataStructureException {
		
		Class desiredClass = propertyInfo.getClazz();
		
		ObjectValue value;
		try {
			value = node.getValue();
		} catch (DeletedException e) {
			return null;
		}
		
		if (!(value instanceof ReferenceValue)) 
			throw new InvalidDataStructureException();
		
		Node targetNode = ((ReferenceValue)value).getTargetNode();		 
		
		Object result = session.
			getDatabaseContext().
			getTransparentProxyFactory().
			createProxy(targetNode, desiredClass, session);
		
		return result;
	}

	@Override
	protected ObjectValue createPrimitiveValue(
			Object data, 
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws InvalidDataStructureException {
		
		if (!session.getDatabaseContext().getTransparentProxyFactory().isProxy(data)) {
			throw new ObjectNotPersistentException();
			//TODO - cascade persist
		} else {
			
			Node targetNode = session.getDatabaseContext().
				getTransparentProxyFactory().getNodeOfProxy(data);

			ObjectValue value = new ReferenceValue(targetNode);
			
			return value;
			
		}
	}
	
}
