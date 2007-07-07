package pl.tzr.transparent.proxy.handler;

import java.util.Set;

import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.proxy.collection.PersistentPrimitiveSet;
import pl.tzr.transparent.structure.model.CollectionPropertyInfo;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class PrimitiveSetPropertyAccessor 
    extends AbstractSetPropertyAccessor 
    implements PropertyAccessor<Set> {

    @Override
    protected Node createItemRepresentation(
            Object item, 
            PropertyInfo propertyInfo, 
            TransparentSession session) {
                
        ObjectValue nodeValue = SimpleValue.build(item);
        
        return new LoximNode(propertyInfo.getNodeName(), nodeValue);
                        
    }

    @Override
    public Set retrieveFromBase(
            Node parent, 
            PropertyInfo propertyInfo, 
            TransparentSession session) 
        throws SBQLException {
    	
    	CollectionPropertyInfo castedPropertyInfo = 
    		(CollectionPropertyInfo)propertyInfo;
                
    			
    	PersistentPrimitiveSet set = new PersistentPrimitiveSet(
                parent, propertyInfo.getNodeName(), 
    			castedPropertyInfo.getItemClass(), session);
    	
    	return set;
    }

}
