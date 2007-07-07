package pl.tzr.browser.session;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import pl.tzr.browser.store.LoximExecutor;
import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.exception.SessionClosedException;

/**
 * Implementation of Session interface to be used with LoXiM database
 * @author Tomasz Rosiek
 *
 */
public class LoximSession implements Session {

	private SimpleConnection connection;
	
	private LoximExecutor executor;

	private Map<String, Node> fetchedNodes = new HashMap<String, Node>();
	
	private boolean active = false;
	
	public LoximSession(SimpleConnection connection) throws SBQLException {
		this.connection = connection;
		this.executor = new LoximExecutor(this);		
		connection.beginTransaction();
		active = true;
	}
	
	public Set<Node> find(String query, ObjectValue... params) 
        throws SBQLException {
		return executor.find(query, params);
	}
	
	public void addToRoot(Node node) throws SBQLException, DeletedException {
		executor.attachObject(node);		
	}	
	
	public SimpleConnection getConnection() {
		return connection;
	}

	public Map<String, Node> getFetchedNodes() {
		return fetchedNodes;
	}
	
	public void commit() {
		if (!active) throw new SessionClosedException();
		try {
			connection.commitTransation();
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}
		onClose();
	}
	
	public void rollback() {
		if (!active) throw new SessionClosedException();
		try {
			connection.rollbackTransaction();
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}
		onClose();
	}
	
	protected void onClose() {
		active = false;
		fetchedNodes.clear();
	}

	public LoximExecutor getExecutor() {
		if (!active) throw new SessionClosedException();
		
		return executor;	
		
	}
	
	public Node createNode(String ref, String name) {
		
		if (fetchedNodes.containsKey(ref)) {
			Node node = fetchedNodes.get(ref);
			return node;
		} else {
			Node node = new LoximNode(this, ref, name);
			fetchedNodes.put(ref, node);
			return  node;
		}				
		
	}

	public boolean isActive() {
		return active;
	}
	
}
