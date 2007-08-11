package pl.tzr.browser.session;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import pl.tzr.browser.store.LoximExecutor;
import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.LoximDatasource;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.exception.SessionClosedException;

/**
 * Implementation of Session interface to be used with LoXiM database.
 * @author Tomasz Rosiek
 *
 */
public class LoximSession implements Session {

    private LoximDatasource datasource;

    private SimpleConnection connection;

    private LoximExecutor executor;

    private Map<String, Node> fetchedNodes = new HashMap<String, Node>();

    private boolean active = false;

    public LoximSession(final LoximDatasource _datasource) 
        throws SBQLException {
        this.datasource = _datasource;
        executor = new LoximExecutor(this);
        connection = datasource.getConnection();
        connection.beginTransaction();
        active = true;
    }

    public Collection<Node> find(
            final String query, 
            final ObjectValue... params) 
        throws SBQLException {
        return executor.find(query, params);
    }

    public Collection<SimpleValue> findPrimitive(
            final String query, 
            final ObjectValue... params)
            throws SBQLException {

        return executor.findPrimitive(query, params);
    }

    public void addToRoot(final Node node) {
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
            try {
                connection.commitTransation();                        
            } catch (SBQLException e) {
                throw new NestedSBQLException(e);
            }
        } finally {
            try {
                datasource.release(connection);                       
            } catch (SBQLException e) {
                throw new NestedSBQLException(e);
            }
            
        }
        onClose();

    }

    public void rollback() {
        if (!active) throw new SessionClosedException();
        try {
            try {
                connection.rollbackTransaction();                        
            } catch (SBQLException e) {
                throw new NestedSBQLException(e);
            }
        } finally {
            try {
                datasource.release(connection);                       
            } catch (SBQLException e) {
                throw new NestedSBQLException(e);
            }
            
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

    public Node createNode(final String ref, final String name) {

        if (fetchedNodes.containsKey(ref)) {
            Node node = fetchedNodes.get(ref);
            return node;
        } else {
            Node node = new LoximNode(this, ref, name);
            fetchedNodes.put(ref, node);
            return  node;
        }

    }
    
    public Node fetchNode(final String ref) {
        try {
            return executor.loadObject(ref);
        } catch (DeletedException e) {
            return null;
        }
    }

    public boolean isActive() {
        return active;
    }

}
