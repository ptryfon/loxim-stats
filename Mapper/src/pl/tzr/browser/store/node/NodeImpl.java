package pl.tzr.browser.store.node;

import java.util.Collection;

import pl.tzr.browser.store.Store;
import pl.tzr.driver.loxim.exception.SBQLException;

public class NodeImpl implements Node {
	private final Store store;
	private final String ref;
	private final String name;

	
	public NodeImpl(Store store, String ref, String name) {
		this.store = store;
		this.ref = ref;
		this.name = name;
	}

	public ObjectValue getValue() throws SBQLException {
		return store.getValue(ref);
	}

	public Collection<Node> getChildNodes(String propertyName)  throws SBQLException {
		return store.getChildNodes(ref, propertyName);
	}
	
	public Node getUniqueChildNode(String propertyName) throws SBQLException {
		Collection<Node> children = store.getChildNodes(ref, propertyName);
		if (children.isEmpty()) return null;
		//FIXME wyłączyć potem
		//if (children.size() > 1) throw new IllegalStateException("Za dużo elementów");
		return children.iterator().next();
	}	

	public void setValue(ObjectValue value) throws SBQLException {
		store.setValue(ref, value);
	}
	
	public void addChild(Node child) throws SBQLException {
		store.addChild(ref, child);
	}
	


	public String getName() {
		return name;
	}

	public void delete() throws SBQLException {
		store.deleteObject(ref);
	}
	
	public String getReference() {
		return ref;
	}



}
