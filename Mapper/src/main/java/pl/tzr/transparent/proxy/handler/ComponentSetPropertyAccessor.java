package pl.tzr.transparent.proxy.handler;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.exception.InvalidOperationException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.proxy.collection.PersistentComponentSet;
import pl.tzr.transparent.structure.annotation.OnBindAction;
import pl.tzr.transparent.structure.annotation.OnRemoveAction;
import pl.tzr.transparent.structure.model.CollectionPropertyInfo;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class ComponentSetPropertyAccessor 
    extends AbstractSetPropertyAccessor 
    implements PropertyAccessor<Set> {
    
    private OnBindAction onBindAction;
    
    /* TODO - currently not supported */
    private OnRemoveAction onRemoveAction;
       
    public ComponentSetPropertyAccessor(
            OnBindAction onBindAction, 
            OnRemoveAction onRemoveAction) {
        super();
        this.onBindAction = onBindAction;
        this.onRemoveAction = onRemoveAction;
    }

    @Override
    protected Node createItemRepresentation(
            Object item, 
            PropertyInfo propertyInfo, 
            TransparentSession session) {
                           
        if (session.getDatabaseContext().getTransparentProxyFactory().isProxy(item)) {
            
            switch (onBindAction) {
            case BIND_INSTANCE:
                throw new UnsupportedOperationException(                                        
                        "Instance rebinding not implemented until rename " +
                        "operation is not supported by LoXiM database");
            case COPY:
                Node newNode = session.getDatabaseContext().
                getModelRegistry().
                createNodeRepresentation(item, propertyInfo.getNodeName(), session);
                return newNode;
            case DENY:
                throw new InvalidOperationException("Biding persistent objects denied");
            default:
                throw new IllegalStateException();
            }
            
        } else {
            Node newNode = session.getDatabaseContext().
            getModelRegistry().
            createNodeRepresentation(item, propertyInfo.getNodeName(), session);
            return newNode;
        }
            
    }

    @Override
    public Set retrieveFromBase(
            Node parent, 
            PropertyInfo propertyInfo, 
            TransparentSession session) {
    	
    	CollectionPropertyInfo castedPropertyInfo = 
    		(CollectionPropertyInfo)propertyInfo;
                
    			
    	PersistentComponentSet set = new PersistentComponentSet(
                parent, propertyInfo.getNodeName(), 
    			castedPropertyInfo.getItemClass(), session);
    	
    	return set;
    }

}
