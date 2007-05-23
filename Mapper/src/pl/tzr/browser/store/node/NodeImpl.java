package pl.tzr.browser.store.node;

import java.util.Collection;

import pl.tzr.browser.session.Session;
import pl.tzr.browser.session.LoximSession;
import pl.tzr.driver.loxim.exception.SBQLException;

public class NodeImpl implements Node {
	private final String ref;
	private final String name;
	
	private LoximSession owningSession;
	
	public NodeImpl(LoximSession loximSession, String ref, String name) {
		this.ref = ref;
		this.name = name;
		this.owningSession = loximSession;
	}

	public ObjectValue getValue() throws SBQLException {
		return owningSession.getExecutor().getValue(ref);
	}

	public Collection<Node> getChildNodes(String propertyName)  throws SBQLException {
		return owningSession.getExecutor().getChildNodes(ref, propertyName);
	}
	
	public Node getUniqueChildNode(String propertyName) throws SBQLException {
		Collection<Node> children = owningSession.getExecutor().
			getChildNodes(ref, propertyName);
		
		if (children.isEmpty()) return null;
		//FIXME wyłączyć potem
		//if (children.size() > 1) throw new IllegalStateException("Za dużo elementów");
		return children.iterator().next();
	}	

	public void setValue(ObjectValue value) throws SBQLException {
		owningSession.getExecutor().setValue(ref, value);
	}
	
	public void addChild(Node child) throws SBQLException {
		owningSession.getExecutor().addChild(ref, child);
	}
	


	public String getName() {
		return name;
	}

	public void delete() throws SBQLException {
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

}
