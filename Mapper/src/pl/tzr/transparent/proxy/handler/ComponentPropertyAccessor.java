package pl.tzr.transparent.proxy.handler;


import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.annotation.OnRemoveAction;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class ComponentPropertyAccessor implements PropertyAccessor {
    
    private OnRemoveAction onRemoveAction = OnRemoveAction.DELETE; 
    
    public ComponentPropertyAccessor() {
        
    }
    
    public ComponentPropertyAccessor(OnRemoveAction onRemoveAction) {
        this.onRemoveAction = onRemoveAction;
    }
		
	public Object retrieveFromBase(
			Node parent, 
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws SBQLException, DeletedException {
		
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
		throws SBQLException, DeletedException {
		
		ObjectValue value = parent.getValue();
		if (!(value instanceof ComplexValue)) throw new InvalidDataStructureException();
		
		Node foundNode = parent.getUniqueChildNode(propertyInfo.getNodeName());
		
		if (foundNode == null) {
			if (data != null) {
				/* Create new node */				
				Node newNode = session.getDatabaseContext().
					getModelRegistry().
					createNodeRepresentation(data, propertyInfo.getNodeName(), session);
				parent.addChild(newNode);
			}
			
		} else {
			if (data != null) {
				/* Delete existing node and create new one */
                
                    
		        detachOldNode(foundNode);
                

				Node newNode = session.getDatabaseContext().
					getModelRegistry().
					createNodeRepresentation(data, propertyInfo.getNodeName(), session);
				parent.addChild(newNode);				
			} else {
				/* Remove existing node */
                detachOldNode(foundNode);
			}
		}		
		
	}		
    
    private void detachOldNode(Node node) throws SBQLException {
               
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
