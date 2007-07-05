package pl.tzr.transparent.proxy.handler;

import java.util.Set;

import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.proxy.collection.PersistentReferenceSet;
import pl.tzr.transparent.structure.model.CollectionPropertyInfo;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class ReferenceSetPropertyAccessor 
    extends AbstractSetPropertyAccessor 
    implements PropertyAccessor<Set> {

    @Override
    protected Node createItemRepresentation(
            Object item, 
            PropertyInfo propertyInfo, 
            TransparentSession session) {
        
        if (!session.getDatabaseContext().
                getTransparentProxyFactory().isProxy(item)) throw 
                new IllegalStateException("Node you want do add is not persistent");
        //TODO add cascade persisting
        
        Node targetNode = session.getDatabaseContext().
            getTransparentProxyFactory().getNodeOfProxy(item);
        
        ObjectValue nodeValue = new ReferenceValue(targetNode);
        
        return new LoximNode(propertyInfo.getNodeName(), nodeValue);
                        
    }

    @Override
    public Set retrieveFromBase(Node parent, PropertyInfo propertyInfo, TransparentSession session) throws SBQLException {
    	
    	CollectionPropertyInfo castedPropertyInfo = 
    		(CollectionPropertyInfo)propertyInfo;
                
    			
    	PersistentReferenceSet set = new PersistentReferenceSet(
                parent, propertyInfo.getNodeName(), 
    			castedPropertyInfo.getItemClass(), session);
    	
    	return set;
    }

}
