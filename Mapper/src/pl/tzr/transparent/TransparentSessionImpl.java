package pl.tzr.transparent;

import java.util.HashSet;
import java.util.Set;

import pl.tzr.browser.session.Session;
import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.transparent.structure.model.ModelRegistry;


public class TransparentSessionImpl implements TransparentSession {
	private final Session session;
	private final TransparentProxyFactory transparentProxyFactory;
	
	public TransparentSessionImpl(
			Session session, 
			TransparentProxyFactory transparentProxyFactory,
			ModelRegistry modelRegistry) {
		this.session = session;
		this.transparentProxyFactory = transparentProxyFactory;
	}

	public void delete(Object object) {
		throw new UnsupportedOperationException();
		// TODO 
	}

	public Set<Object> find(String query, Class desiredClass) throws SBQLException {
		
		Set<Node> results = session.find(query);
		
		Set<Object> transparentResults = new HashSet<Object>();
		
		for (Node result : results) {
			Object transparentResult = transparentProxyFactory.createProxy(result, desiredClass);
			transparentResults.add(transparentResult);
		}
		
		return transparentResults;
	}

	public void persist(Object object) {
		throw new UnsupportedOperationException();
		/* Zapisuje obiekt w bazie danych */
		
		/* Modyfikuje działanie getterow i setterów obiektu "object" za pomocą CGLIB'a */

	}

}
