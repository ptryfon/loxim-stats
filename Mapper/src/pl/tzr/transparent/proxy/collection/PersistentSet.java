package pl.tzr.transparent.proxy.collection;

import java.util.Collection;
import java.util.Iterator;
import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.exception.TransparentDeletedException;
import pl.tzr.transparent.TransparentSession;

public class PersistentSet<E> implements Set<E> {
	
	private final Node parentNode;
	
	private final String propertyName;
	
	private final TransparentSession session;
	
	private final Class clazz;
		
	private class CustomIterator implements Iterator<E> {
		
		private Iterator<Node> nodeIterator;
		
		private Node lastNode;
		
		public CustomIterator(Collection<Node> nodes) {
						
			nodeIterator = nodes.iterator();
			
		}

		public boolean hasNext() {
			return nodeIterator.hasNext();
		}

		public E next() {
			
			lastNode = nodeIterator.next();
			
			if (lastNode == null) return null;
			
			@SuppressWarnings("unchecked")
			E transparentProxy = (E)createProxy(lastNode);
			
			return transparentProxy;
		}

		public void remove() {
			
			if (lastNode == null) throw new IllegalStateException();
			
			try {
				lastNode.delete();
			} catch (SBQLException e) {
				throw new NestedSBQLException(e);
			}
			nodeIterator.remove();
			
		}
		
	}
		
	public PersistentSet(final Node parentNode, final String propertyName, 
			final Class clazz, 
			final TransparentSession session) {
		
		this.parentNode = parentNode;
		this.propertyName = propertyName;
		this.session = session;
		this.clazz = clazz;
	}

	private Object createProxy(Node node) {
		
		Object proxy = session.getDatabaseContext().
            getTransparentProxyFactory().createProxy(node, clazz, session);		
		return proxy;
		
	}

	private Collection<Node> fetchChildren(){
               		
		try {
            
            return parentNode.getChildNodes(propertyName);
		
		} catch (SBQLException e) {
			throw new RuntimeException(e);
		}
		
	}	

	
	public boolean contains(Object arg) {
		if (!session.getDatabaseContext().
                getTransparentProxyFactory().isProxy(arg)) return false;
        
		Node node = session.getDatabaseContext().
            getTransparentProxyFactory().getNodeOfProxy(arg);
						
		return (parentNode.isChild(node));
		
	}

	public boolean containsAll(Collection args) {
		
		for (Object arg : args) {
			
			if (!contains(arg)) return false;
			
		}
		return true;
	}

	public boolean isEmpty() {
				
		return (parentNode.childAmount(propertyName) == 0);
				
	}

	public Iterator<E> iterator() {
		return new CustomIterator(fetchChildren());
	}
	
	public boolean add(Object item) {
		
		Node node = session.getDatabaseContext().getModelRegistry().
			createNodeRepresentation(item, propertyName, session);		
		
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

	public boolean addAll(Collection args) {
		boolean isChanged = false;
		for (Object arg : args) {
			isChanged = isChanged || add(arg);
		}		
		return isChanged;
	}

	public void clear() {
		try {
			parentNode.removeAllChildren(propertyName);
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}

		throw new UnsupportedOperationException();		
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

	public boolean removeAll(Collection args) {
		boolean isChanged = false;
		for (Object arg : args) {
			isChanged = isChanged || remove(arg);
		}		
		return isChanged;
	}

	public boolean retainAll(Collection collection) {
		
		boolean isChanged = false;
		Iterator<E> i = this.iterator();
		while (i.hasNext()) {
			E item = i.next();
			if (!collection.contains(item)) {
				isChanged = true;
				i.remove();
			}
		}
		
		return isChanged;				
	}

	public int size() {
        return parentNode.childAmount(propertyName);
	}

	public Object[] toArray() {
		Object[] array = new Object[size()];
		return toArray(array);
	}

	public <T> T[] toArray(T[] array) {
		int i = 0;
		for (E item : this) {
			
			@SuppressWarnings("unchecked")
			T castedItem = (T)item;
			
			array[i] = castedItem; 
			i++;
		}

		return array;		
	}

}
