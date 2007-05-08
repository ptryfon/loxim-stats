package pl.tzr.transparent.proxy.collection;

import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.transparent.TransparentProxyFactory;

public class PersistentSet implements Set {
	
	private final Node parentNode;
	
	private final String propertyName;
	
	private final TransparentProxyFactory transparentProxyFactory;
	
	private final Class clazz;
	
	private Set<Node> itemNodes;
	
	private class CustomIterator implements Iterator {
		
		private Iterator<Node> nodeIterator;
		
		public CustomIterator() {
			
			fetchChildren();			
			nodeIterator = itemNodes.iterator();
			
		}

		public boolean hasNext() {
			return nodeIterator.hasNext();
		}

		public Object next() {
			
			Node node = nodeIterator.next();
			
			if (node == null) return null;
			
			Object transparentProxy = createProxy(node);
			
			return transparentProxy;
		}

		public void remove() {
			throw new UnsupportedOperationException();
			
		}
		
	}
		
	public PersistentSet(final Node parentNode, final String propertyName, final Class clazz, final TransparentProxyFactory transparentProxyFactory) {
		super();
		this.parentNode = parentNode;
		this.propertyName = propertyName;
		this.transparentProxyFactory = transparentProxyFactory;
		this.clazz = clazz;
	}

	private Object createProxy(Node node) {
		
		Object proxy = transparentProxyFactory.createProxy(node, clazz);		
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
		if (!transparentProxyFactory.isProxy(arg)) return false;
				
		fetchChildren();			
		Node node = transparentProxyFactory.getNodeOfProxy(arg);
						
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

	public Iterator iterator() {
		return new CustomIterator();
	}
	
	public boolean add(Object arg0) {
		//TODO
		throw new UnsupportedOperationException();
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

	public Object[] toArray(Object[] array) {
		int i = 0;
		for (Object item : this) {
			array[i] = item;
			i++;
		}

		return array;		
	}

}
