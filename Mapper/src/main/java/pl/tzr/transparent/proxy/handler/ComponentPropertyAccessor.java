package pl.tzr.transparent.proxy.handler;


import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.exception.InvalidOperationException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.annotation.OnBindAction;
import pl.tzr.transparent.structure.annotation.OnRemoveAction;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class ComponentPropertyAccessor implements PropertyAccessor {
    
    private OnBindAction onBindAction = OnBindAction.COPY;
    
    private OnRemoveAction onRemoveAction = OnRemoveAction.DELETE;
    
        
    public ComponentPropertyAccessor(
            OnBindAction onBindAction, 
            OnRemoveAction onRemoveAction) {
        this.onRemoveAction = onRemoveAction;
        this.onBindAction = onBindAction;
    }
		
	public Object retrieveFromBase(
			Node parent, 
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws DeletedException {
                
		
		Class desiredClass = propertyInfo.getClazz();
		
		ObjectValue value = parent.getValue();
		
		if (!(value instanceof ComplexValue))  
			throw new InvalidDataStructureException();
		
		Node foundProxy = parent.getUniqueChildNode(propertyInfo.getNodeName());
		
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
		throws DeletedException {
		
		ObjectValue value = parent.getValue();
		if (!(value instanceof ComplexValue)) throw new InvalidDataStructureException();
		
		Node foundNode = parent.getUniqueChildNode(propertyInfo.getNodeName());
		
		if (foundNode == null) {
			if (data != null) {
                
                /* Create new node */				
                Node newNode = attachNewNode(data, parent, propertyInfo, session);
                parent.addChild(newNode);
			}
			
		} else {
			if (data != null) {
				/* Delete existing node and create new one */
                                    
		        detachOldNode(foundNode);

				Node newNode = attachNewNode(data, parent, propertyInfo, session);
				parent.addChild(newNode);				
			} else {
				/* Remove existing node */
                detachOldNode(foundNode);
			}
		}		
		
	}	
    
    private Node attachNewNode(Object data, 
            Node parent, 
            PropertyInfo propertyInfo, 
            TransparentSession session) {
        
        if (session.getDatabaseContext().getTransparentProxyFactory().isProxy(data)) {
            
            switch (onBindAction) {
            case BIND_INSTANCE:
                throw new UnsupportedOperationException(
                        "Instance rebinding not implemented until rename " +
                        "operation is not supported by LoXiM database");
            case COPY:
                Node newNode = session.getDatabaseContext().
                getModelRegistry().
                createNodeRepresentation(data, propertyInfo.getNodeName(), session);
                return newNode;
            case DENY:
                throw new InvalidOperationException("Biding persistent objects denied");
            default:
                throw new IllegalStateException();
            }
            
        } else {
            Node newNode = session.getDatabaseContext().
            getModelRegistry().
            createNodeRepresentation(data, propertyInfo.getNodeName(), session);
            return newNode;
        }

    }
    
    private void detachOldNode(Node node) {
               
        switch (onRemoveAction) {
            case DELETE:
                node.delete();
                break;
            case MOVE_TO_ROOT:
                throw new UnsupportedOperationException();
            default:
                throw new UnsupportedOperationException();
        }
        
    }
	
}
