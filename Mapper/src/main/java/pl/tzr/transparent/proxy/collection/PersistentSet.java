package pl.tzr.transparent.proxy.collection;

import java.util.Collection;
import java.util.Iterator;
import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.transparent.TransparentSession;

public abstract class PersistentSet<E> implements Set<E> {

    protected final Node parentNode;
    protected final String nodeName;
    protected final TransparentSession session;
    protected final Class clazz;
    
protected class CustomIterator implements Iterator<E> {
        
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
            E transparentProxy = (E)createNodeRepresentation(lastNode);
            
            return transparentProxy;
        }

        public void remove() {
            
            if (lastNode == null) throw new IllegalStateException();
                        
            lastNode.delete();
            nodeIterator.remove();
            
        }
        
    }    

    public PersistentSet(final Node parentNode, final String propertyName, 
            final Class clazz, 
            final TransparentSession session) {
        
        this.parentNode = parentNode;
        this.nodeName = propertyName;
        this.session = session;
        this.clazz = clazz;       
    }
    
    protected abstract Object createNodeRepresentation(Node node);
    
    public abstract boolean add(Object item);
    
    protected Collection<Node> fetchChildren(){
        
        return parentNode.getChildNodes(nodeName);                
        
    }    
    
    public Iterator<E> iterator() {
        return new CustomIterator(fetchChildren());
    }    

    public boolean containsAll(Collection args) {
    	
    	for (Object arg : args) {
    		
    		if (!contains(arg)) return false;
    		
    	}
    	return true;
    }

    public boolean addAll(Collection args) {
    	boolean isChanged = false;
    	for (Object arg : args) {
    		isChanged = isChanged || add(arg);
    	}		
    	return isChanged;
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
        return parentNode.childAmount(nodeName);
    }
    
    public void clear() {
        parentNode.removeAllChildren(nodeName);
    }
    
    public boolean isEmpty() {
        
        return (parentNode.childAmount(nodeName) == 0);
                
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