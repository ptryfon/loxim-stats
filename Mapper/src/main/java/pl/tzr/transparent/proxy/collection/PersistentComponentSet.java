package pl.tzr.transparent.proxy.collection;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.transparent.TransparentSession;

public class PersistentComponentSet<E> extends PersistentSet<E> implements Set<E> {
				
	public PersistentComponentSet(final Node parentNode, final String propertyName, 
			final Class clazz, 
			final TransparentSession session) {
		
		super(parentNode, propertyName, clazz, session);
	}

	@Override
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
	
	@Override
    public boolean add(Object item) {
		
		Node node = session.getDatabaseContext().getModelRegistry().
			createNodeRepresentation(item, nodeName, session);		
		
			
		if (!node.isDetached() && parentNode.isChild(node)) {
            
			return false;
			
		} else {

			parentNode.addChild(node);
			
			return true;
			
		}		
			
		
									
	}

	

	public boolean remove(Object item) {
		
	    if (!session.getDatabaseContext().getTransparentProxyFactory().isProxy(item)) 
            return false;
        
        Node node = session.getDatabaseContext().
            getTransparentProxyFactory().getNodeOfProxy(item);
		

		if (!parentNode.isChild(node)) {
			
			/* Apparently cannot happen */				
			return false;
			
		} else {

			node.delete();				
			
			return true;
			
		}
			
				
	}
	

}
