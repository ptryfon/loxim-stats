package pl.tzr.transparent.proxy.collection;

import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.exception.TransparentDeletedException;
import pl.tzr.transparent.TransparentSession;

public class PersistentReferenceSet<E> implements Set<E> {
	
	private final Node parentNode;
	
	private final String propertyName;
	
	private final TransparentSession session;
	
	private final Class clazz;
	
	private Set<Node> itemNodes;
	
	private class CustomIterator implements Iterator<E> {
		
		private Iterator<Node> nodeIterator;
		
		private Node lastNode;
		
		public CustomIterator() {
			
			fetchChildren();			
			nodeIterator = itemNodes.iterator();
			
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
		
	public PersistentReferenceSet(final Node parentNode, final String propertyName, 
			final Class clazz, 
			final TransparentSession session) {
		
		this.parentNode = parentNode;
		this.propertyName = propertyName;
		this.session = session;
		this.clazz = clazz;
	}

	private Object createProxy(Node node) {
		
		try {
			
			//FIXME error handling when node is not reference
			Node targetNode = ((ReferenceValue)(node.getValue())).getTargetNode();
			Object proxy = session.getDatabaseContext().getTransparentProxyFactory().createProxy(targetNode, clazz, session);		
			return proxy;
			
		} catch (DeletedException e) {
			throw new TransparentDeletedException(e);
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}				
		
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
		
		//FIXME find node containing pointer
						
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
		
		if (!session.getDatabaseContext().getTransparentProxyFactory().isProxy(item)) {
			throw new UnsupportedOperationException("Item is not yet persisted");
		} 
		
		Node targetNode = session.getDatabaseContext().getTransparentProxyFactory().getNodeOfProxy(item);
		
		Node collectionItemNode = new LoximNode(propertyName, new ReferenceValue(targetNode));
				
		try {
			
			//FIXME find node containing pointer
			
			if (parentNode.isChild(targetNode)) {
				
				return false;
				
			} else {

				parentNode.addChild(collectionItemNode);
				
				if (itemNodes != null) itemNodes.add(collectionItemNode);
				
				return true;
				
			}		
			
		
		} catch (SBQLException e) {
			
			throw new NestedSBQLException(e);
			
		} catch (DeletedException e) {
			
			throw new TransparentDeletedException(e);
			
		}
							
		/* TODO - what if there is any iterator ? */
		
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
		
		if (!session.getDatabaseContext().getTransparentProxyFactory().isProxy(item)) {
			throw new UnsupportedOperationException("Item is not yet persisted");
		} 
		
		Node targetNode = session.getDatabaseContext().getTransparentProxyFactory().getNodeOfProxy(item);
		
		Node itemNode = null; /* TODO */
		
		try {

			if (!parentNode.isChild(targetNode)) {
							
				return false;
				
			} else {
	
				targetNode.delete();				
				if (itemNodes != null) itemNodes.remove(itemNode);				
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
