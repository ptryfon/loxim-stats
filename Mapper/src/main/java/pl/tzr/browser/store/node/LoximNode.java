package pl.tzr.browser.store.node;

import java.util.Collection;
import java.util.LinkedList;
import java.util.List;

import pl.tzr.browser.session.LoximSession;
import pl.tzr.browser.session.Session;

public class LoximNode implements Node {

    private String ref;
	
	private String name;
	
	private boolean detached = false;
	
	private ObjectValue detachedValue;
	
	private List<Node> detachedChildren = new LinkedList<Node>();
	
	private LoximSession owningSession;
	
	public LoximNode(LoximSession loximSession, String ref, String name) {
		this.ref = ref;
		this.name = name;
		this.owningSession = loximSession;
	}
	
	public LoximNode(String name, ObjectValue value) {
		this.name = name;
		this.detachedValue = value;
		this.detached = true;
	}

	public ObjectValue getValue() {
		
		if (detached) {
			return detachedValue;
		} else {
			return owningSession.getExecutor().getValue(ref);
		}
				
	}
	
	public void markAttached(final LoximSession session, final String _ref) {
		this.ref = _ref;
		this.owningSession = session;
		detached = false;
	}

	public Collection<Node> getChildNodes(String propertyName)  {
		
		if (detached) {
			List<Node> childrenByName = new LinkedList<Node>();

			for (Node child : detachedChildren) {
				
				if (child.getName().equals(propertyName)) {
					childrenByName.add(child);
				}
				
			}

			return childrenByName;
		} else {
			return owningSession.getExecutor().getChildNodes(ref, propertyName);	
		}
		
		
	}
	
	public Node getUniqueChildNode(String propertyName) {
		
		if (detached) {
			
			for (Node child : detachedChildren) {
				
				if (child.getName().equals(propertyName)) {
					return child;
				}
				
			}

			return null;
			
		} else {
			Collection<Node> children = owningSession.getExecutor().
				getChildNodes(ref, propertyName);
			
			if (children.isEmpty()) return null;

			if (children.size() > 1) throw new IllegalStateException("Too many elements");
			return children.iterator().next();
		}
		
	}	

	public void setValue(ObjectValue value) {
		
		if (detached) {
			detachedValue = value;
		} else {
			owningSession.getExecutor().setValue(ref, value);	
		}
		
		
	}
	
	public void addChild(Node child) {
		if (detached) {
			detachedChildren.add(child);
		} else {
			owningSession.getExecutor().addChild(ref, child);
		}
	}	

	public String getName() {
		return name;
	}

	public void delete() {
		if (detached) throw new IllegalStateException();
		
		owningSession.getExecutor().deleteObject(ref);
	}
	
	public String getReference() {
		return ref;
	}

	public Session getOwningSession() {
		return owningSession;
	}

	public void setOwningSession(LoximSession owningSession) {
		this.owningSession = owningSession;
	}

	public Collection<Node> getAllChildNodes() {
		if (detached) {
			return detachedChildren;
		} else throw new UnsupportedOperationException();
	}

	public boolean isDetached() {
		return detached;
	}
	
	public boolean isChild(Node childNode) {
				
		return owningSession.getExecutor().isChild(this, childNode);
	}
    
	public int childAmount(String childName) {
        return owningSession.getExecutor().childAmount(this, childName);
    }

    public void removeAllChildren(String childName) {
		
		owningSession.getExecutor().removeAllChildren(this, childName);
		
	}
    
    public Collection<Node> hasChildOfValue(String childName, 
            ObjectValue value) {
        
        return owningSession.getExecutor().findChildsOfValue(
                this, childName, value);
    }    
	

}
