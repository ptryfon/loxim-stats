package pl.tzr.transparent.proxy.collection;

import java.util.Collection;
import java.util.HashSet;
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
	
	private Set<Node> itemNodes;
	
	private class CustomIterator implements Iterator<E> {
		
		private Iterator<Node> nodeIterator;
		
		public CustomIterator() {
			
			fetchChildren();			
			nodeIterator = itemNodes.iterator();
			
		}

		public boolean hasNext() {
			return nodeIterator.hasNext();
		}

		public E next() {
			
			Node node = nodeIterator.next();
			
			if (node == null) return null;
			
			@SuppressWarnings("unchecked")
			E transparentProxy = (E)createProxy(node);
			
			return transparentProxy;
		}

		public void remove() {
			throw new UnsupportedOperationException();
			
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
		
		Object proxy = session.getDatabaseContext().getTransparentProxyFactory().createProxy(node, clazz, session);		
		return proxy;
		
	}

	private void fetchChildren(){
		
		try {
		
			if (itemNodes == null) {			
				itemNodes = new HashSet<Node>(parentNode.getChildNodes(propertyName));			
			}
		
		} catch (SBQLException e) {
			throw new RuntimeException(e);
		}
		
	}	

	
	public boolean contains(Object arg) {
		if (!session.getDatabaseContext().getTransparentProxyFactory().isProxy(arg)) return false;
				
		fetchChildren();			
		Node node = session.getDatabaseContext().getTransparentProxyFactory().getNodeOfProxy(arg);
						
		return (itemNodes.contains(node));
		
	}

	public boolean containsAll(Collection args) {
		
		for (Object arg : args) {
			
			if (!contains(arg)) return false;
			
		}
		return true;
	}

	public boolean isEmpty() {
		
		fetchChildren();		
		return (itemNodes.isEmpty());
				
	}

	public Iterator<E> iterator() {
		return new CustomIterator();
	}
	
	public boolean add(Object item) {
		
		Node node = session.getDatabaseContext().getModelRegistry().
			createNodeRepresentation(item, propertyName, session);
		
		try {
		
			parentNode.addChild(node);
		
		} catch (SBQLException e) {
			
			throw new NestedSBQLException(e);
			
		} catch (DeletedException e) {
			
			throw new TransparentDeletedException(e);
			
		}
		
		if (itemNodes != null) itemNodes.add(node);
		
		/* TODO - what if there is any iterator ? */
		
		/* TODO - what if the item already exists in the set ? */
		
		return true;
		
	}

	public boolean addAll(Collection args) {
		boolean isChanged = false;
		for (Object arg : args) {
			isChanged = isChanged || add(arg);
		}		
		return isChanged;
	}

	public void clear() {
		//TODO
		throw new UnsupportedOperationException();		
	}	

	public boolean remove(Object arg0) {
		//TODO
		throw new UnsupportedOperationException();		
	}

	public boolean removeAll(Collection args) {
		boolean isChanged = false;
		for (Object arg : args) {
			isChanged = isChanged || remove(arg);
		}		
		return isChanged;
	}

	public boolean retainAll(Collection arg0) {
		//TODO
		throw new UnsupportedOperationException();		
	}

	public int size() {		
		fetchChildren();
		return (itemNodes.size());
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
