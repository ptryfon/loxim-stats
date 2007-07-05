package pl.tzr.transparent.proxy.handler;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.model.PropertyInfo;

public abstract class AbstractSetPropertyAccessor {

    public abstract Set retrieveFromBase(
            Node parent, 
            PropertyInfo propertyInfo, 
            TransparentSession session) 
        throws SBQLException;

    public void saveToBase(
            Set data, 
            Node parent, 
            PropertyInfo propertyInfo, 
            TransparentSession session) 
        throws SBQLException, DeletedException {
    								
    	for (Object item : data) {
    		
    		Node itemNode = createItemRepresentation(item, propertyInfo, session);
    		
    		parent.addChild(itemNode);
    		
    	}				
    
    }
    
    protected abstract Node createItemRepresentation(
            Object item, 
            PropertyInfo propertyInfo, 
            TransparentSession session);

}