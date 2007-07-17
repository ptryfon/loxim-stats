package pl.tzr.transparent.proxy.handler;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.proxy.collection.PersistentComponentSet;
import pl.tzr.transparent.structure.model.CollectionPropertyInfo;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class ComponentSetPropertyAccessor 
    extends AbstractSetPropertyAccessor 
    implements PropertyAccessor<Set> {

    @Override
    protected Node createItemRepresentation(
            Object item, 
            PropertyInfo propertyInfo, 
            TransparentSession session) {

        return session.getDatabaseContext().getModelRegistry().
        createNodeRepresentation(item, propertyInfo.getNodeName(), session);
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
