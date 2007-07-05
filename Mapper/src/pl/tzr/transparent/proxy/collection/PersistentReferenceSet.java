package pl.tzr.transparent.proxy.collection;

import java.util.Collection;

import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.exception.TransparentDeletedException;
import pl.tzr.transparent.TransparentSession;

public class PersistentReferenceSet<E> extends PersistentSet<E> {
				
	public PersistentReferenceSet(
            final Node parentNode, 
            final String propertyName, 
			final Class clazz, 
			final TransparentSession session) {
		
		super(parentNode, propertyName, clazz, session);
	}

	protected Object createNodeRepresentation(Node node) {
        
        try {
            Node targetNode = ((ReferenceValue)node.getValue()).getTargetNode();
            Object proxy = session.getDatabaseContext().
                getTransparentProxyFactory().createProxy(targetNode, clazz, session);
            
            return proxy;
            
        } catch (ClassCastException e) {
            throw new InvalidDataStructureException();
        } catch (SBQLException e) {
            throw new NestedSBQLException(e);
        } catch (DeletedException e) {
            return null;
        }
		
	}
		
	public boolean contains(Object arg) {
		if (!session.getDatabaseContext().
                getTransparentProxyFactory().isProxy(arg)) return false;
        
		Node node = session.getDatabaseContext().
            getTransparentProxyFactory().getNodeOfProxy(arg);
        
        Collection<Node> foundNodes = (parentNode.hasChildOfValue(nodeName, 
                new ReferenceValue(node))); 
						
		return (!foundNodes.isEmpty());
		
	}
	
	public boolean add(Object item) {
        
        if (!session.getDatabaseContext().
                getTransparentProxyFactory().isProxy(item)) throw 
                new IllegalStateException("Node you want do add is not persistent");
        //TODO add cascade persisting
        
        Node targetNode = session.getDatabaseContext().
            getTransparentProxyFactory().getNodeOfProxy(item);
        
        ObjectValue nodeValue = new ReferenceValue(targetNode);
        
				
		
		try {
			
			if (!parentNode.hasChildOfValue(nodeName, nodeValue).isEmpty()) {
                
				return false;
				
			} else {
                
                Node node = new LoximNode(nodeName, nodeValue);

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
        
        Node targetNode = session.getDatabaseContext().
            getTransparentProxyFactory().getNodeOfProxy(item);
    
        ObjectValue nodeValue = new ReferenceValue(targetNode);
        
		try {
            
            Collection<Node> pointingNodes = 
                parentNode.hasChildOfValue(nodeName, nodeValue);

			if (pointingNodes.isEmpty()) {
							
				return false;
				
			} else {
                
                for (Node pointingNode : pointingNodes) {
                    pointingNode.delete();
                }
	
				return true;
				
			}
			
		} catch (SBQLException e) {
			
			throw new NestedSBQLException(e);
			
		} 
				
	}

}
