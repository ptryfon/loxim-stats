package pl.tzr.transparent;

import java.util.Collection;
import java.util.LinkedList;

import pl.tzr.browser.session.Session;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.exception.ObjectNotPersistentException;

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
		
		databaseContext.getTransparentProxyFactory().getNodeOfProxy(object).delete();
        
	}

	public Collection<Object> find(String query, Class desiredClass) 
        throws SBQLException {
		
		Collection<Node> results = session.find(query);
		
		Collection<Object> transparentResults = new LinkedList<Object>();
		
		for (Node result : results) {
			Object transparentResult = 
                databaseContext.getTransparentProxyFactory().
                createProxy(result, desiredClass, this);
            
			transparentResults.add(transparentResult);
		}
		
		return transparentResults;
	}
    
    public Collection<Object> findWithParams(
            String query, 
            Class desiredClass, Object... params) 
            throws SBQLException {
        
        ObjectValue[] paramValues = buildParamValues(params); 
        
        Collection<Node> results = session.find(query, paramValues);
        
        Collection<Object> transparentResults = new LinkedList<Object>();
        
        for (Node result : results) {
            Object transparentResult = 
                databaseContext.getTransparentProxyFactory().
                createProxy(result, desiredClass, this);
            
            transparentResults.add(transparentResult);
        }
        
        return transparentResults;
    }    
    
    public Collection<Object> findPrimitiveWithParams(
            String query, Object... params) 
            throws SBQLException {
        
        ObjectValue[] paramValues = buildParamValues(params); 
        
        Collection<SimpleValue> results = session.findPrimitive(query, paramValues);
                
        Collection<Object> realResults = new LinkedList<Object>();
        
        for (SimpleValue result : results) {
            
            realResults.add(result.getValue());
        }
        
        return realResults;
    }    
    
    private ObjectValue[] buildParamValues(Object... params) {
        
        ObjectValue[] values = new ObjectValue[params.length];
        
        int i = 0;
        for (Object param : params) {
            
            if (getDatabaseContext().getTransparentProxyFactory().isProxy(param)) {
                
                values[i] = new ReferenceValue(getDatabaseContext().
                        getTransparentProxyFactory().getNodeOfProxy(param));
                
            } else {
                values[i] = SimpleValue.build(param);
            }
                        
            i++;
        }
        
        return values;
    }

	public Object persist(Object object) {
					
		//Store object in the database
		Node node = storeObject(object);
		
		//Create proxy
		Object proxy = databaseContext.getTransparentProxyFactory().
            createRootProxy(node, this);
		
		return proxy;		

	}
	
	private Node storeObject(Object object) {
			
		Node node = databaseContext.getModelRegistry().
            createNodeRepresentation(object, this);
		
		try {
			session.addToRoot(node);
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
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

    public Object getById(String id, Class desiredClass) {
        
        Node foundNode = session.fetchNode(id);
        
        if (foundNode == null) return null;
        
        Object transparentResult = 
            databaseContext.getTransparentProxyFactory().
            createProxy(foundNode, desiredClass, this);
                
        return transparentResult;
    }

    public String getId(Object object) throws ObjectNotPersistentException {
        
        if (!databaseContext.getTransparentProxyFactory().isProxy(object))
            throw new ObjectNotPersistentException();
        
        return databaseContext.getTransparentProxyFactory().
            getNodeOfProxy(object).getReference();
        
    }

}
