package pl.tzr.transparent;

import java.util.HashSet;
import java.util.Set;

import pl.tzr.browser.session.Session;
import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;

/**
 * Implementation of TransparentSession wich uses provided Session and
 * TransparentProxyFactory 
 * @author Tomasz Rosiek 
 *
 */
public class TransparentSessionImpl implements TransparentSession {
	private final Session sessionImpl;
	private final TransparentProxyFactory transparentProxyFactory;
	
	public TransparentSessionImpl(
			Session sessionImpl, 
			TransparentProxyFactory transparentProxyFactory) {
		this.sessionImpl = sessionImpl;
		this.transparentProxyFactory = transparentProxyFactory;
	}

	public void delete(Object object) {
		throw new UnsupportedOperationException();
		// TODO 
	}

	public Set<Object> find(String query, Class desiredClass) throws SBQLException {
		
		Set<Node> results = sessionImpl.find(query);
		
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

	public Session getSession() {
		return sessionImpl;
	}

	public void commit() {
		sessionImpl.commit();
	}

	public boolean isActive() {
		return sessionImpl.isActive();
	}

	public void rollback() {
		sessionImpl.rollback();		
	}

}
