package pl.tzr.browser.session;

import java.util.Set;

import pl.tzr.browser.store.LoximStore;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.exception.SBQLException;

public class SessionImpl implements Session {
	
	private final LoximStore store;

	public SessionImpl(SimpleConnection connection) {
		store = new LoximStore(connection);
	}

	public Set<Node> find(String query) throws SBQLException {
		
		Set<Node> results = store.executeQuery(query);
		
		return results;
	}

	public Node createBoolean(String name, boolean value) throws SBQLException {
		return store.createObject(name, new SimpleValue(value));
	}

	public Node createInt(String name, int value) throws SBQLException {
		return store.createObject(name, new SimpleValue(value));
	}

	public Node createString(String name, String value) throws SBQLException {
		return store.createObject(name, new SimpleValue(value));
	}

}
