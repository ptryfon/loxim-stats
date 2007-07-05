package pl.tzr.transparent.proxy.collection;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.exception.TransparentDeletedException;
import pl.tzr.transparent.TransparentSession;

public class PersistentComponentSet<E> extends PersistentSet<E> implements Set<E> {
				
	public PersistentComponentSet(final Node parentNode, final String propertyName, 
			final Class clazz, 
			final TransparentSession session) {
		
		super(parentNode, propertyName, clazz, session);
	}

	protected Object createNodeRepresentation(Node node) {
		
		Object proxy = session.getDatabaseContext().
            getTransparentProxyFactory().createProxy(node, clazz, session);		
		return proxy;
		
	}
		
	public boolean contains(Object arg) {
		if (!session.getDatabaseContext().
                getTransparentProxyFactory().isProxy(arg)) return false;
        
		Node node = session.getDatabaseContext().
            getTransparentProxyFactory().getNodeOfProxy(arg);
						
		return (parentNode.isChild(node));
		
	}
	
	public boolean add(Object item) {
		
		Node node = session.getDatabaseContext().getModelRegistry().
			createNodeRepresentation(item, nodeName, session);		
		
		try {
			
			if (!node.isDetached() && parentNode.isChild(node)) {
                
				return false;
				
			} else {

				parentNode.addChild(node);
				
				return true;
				
			}		
			
		
		} catch (SBQLException e) {
			
			throw new NestedSBQLException(e);
			
		} catch (DeletedException e) {
			
			throw new TransparentDeletedException(e);
			
		}
									
	}

	

	public boolean remove(Object item) {
		
	    if (!session.getDatabaseContext().getTransparentProxyFactory().isProxy(item)) 
            return false;
        
        Node node = session.getDatabaseContext().
            getTransparentProxyFactory().getNodeOfProxy(item);
		
		try {

			if (!parentNode.isChild(node)) {
				
				/* Apparently cannot happen */				
				return false;
				
			} else {
	
				node.delete();				
				
				return true;
				
			}
			
		} catch (SBQLException e) {
			
			throw new NestedSBQLException(e);
			
		} 
				
	}
	

}
