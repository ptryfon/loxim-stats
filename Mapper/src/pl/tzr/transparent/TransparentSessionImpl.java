package pl.tzr.transparent;

import java.util.HashSet;
import java.util.Set;

import pl.tzr.browser.session.Session;
import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.exception.TransparentDeletedException;

/**
 * Implementation of TransparentSession wich uses provided Session and
 * TransparentProxyFactory 
 * @author Tomasz Rosiek 
 *
 */
public class TransparentSessionImpl implements TransparentSession {
	private final Session session;
	private final DatabaseContext databaseContext;
	
	public TransparentSessionImpl(
			Session session, DatabaseContext databaseContext) {
		this.session = session;
		this.databaseContext = databaseContext;
	}

	public void delete(Object object) {
		if (!databaseContext.getTransparentProxyFactory().isProxy(object))
			throw new IllegalStateException("Object is not persistent");
		
		try {
			databaseContext.getTransparentProxyFactory().getNodeOfProxy(object).delete();
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}
	}

	public Set<Object> find(String query, Class desiredClass) throws SBQLException {
		
		Set<Node> results = session.find(query);
		
		Set<Object> transparentResults = new HashSet<Object>();
		
		for (Node result : results) {
			Object transparentResult = databaseContext.getTransparentProxyFactory().createProxy(result, desiredClass, this);
			transparentResults.add(transparentResult);
		}
		
		return transparentResults;
	}

	public Object persist(Object object) {
					
		//Store object in the database
		Node node = storeObject(object);
		
		//Create proxy
		Object proxy = databaseContext.getTransparentProxyFactory().createRootProxy(node, this);
		
		return proxy;		

	}
	
	private Node storeObject(Object object) {
			
		Node node = databaseContext.getModelRegistry().createNodeRepresentation(object, this);
		
		try {
			session.addToRoot(node);
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		} catch (DeletedException e) {
			throw new TransparentDeletedException(e);
		}
		
		return node;
	}
	
	

	public Session getSession() {
		return session;
	}

	public void commit() {
		session.commit();
	}

	public boolean isActive() {
		return session.isActive();
	}

	public void rollback() {
		session.rollback();		
	}

	public DatabaseContext getDatabaseContext() {
		return databaseContext;
	}

}
