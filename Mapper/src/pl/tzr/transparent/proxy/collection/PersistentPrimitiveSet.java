package pl.tzr.transparent.proxy.collection;

import java.util.Collection;

import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.exception.TransparentDeletedException;
import pl.tzr.transparent.TransparentSession;

public class PersistentPrimitiveSet extends PersistentSet {

    public PersistentPrimitiveSet(
            Node parentNode, 
            String propertyName, 
            Class clazz, 
            TransparentSession session) {
        
        super(parentNode, propertyName, clazz, session);

    }
    


    @Override
    public boolean add(Object item) {
                             
        ObjectValue nodeValue = SimpleValue.build(item);
                                
        try {
            
            if (!parentNode.hasChildOfValue(nodeName, nodeValue).isEmpty()) {
                
                return false;
                
            } else {
                
                Node node = new LoximNode(nodeName, nodeValue);

                parentNode.addChild(node);
                
                return true;
                
            }       
            
        
        } catch (SBQLException e) {
            
            throw new NestedSBQLException(e);
            
        } catch (DeletedException e) {
            
            throw new TransparentDeletedException(e);
            
        }        
        
    }

    @Override
    protected Object createNodeRepresentation(Node node) {
                        
        try {
            
            return ((SimpleValue)node.getValue()).getValue();   
                        
        } catch (SBQLException e) {
            throw new NestedSBQLException(e);
        } catch (DeletedException e) {
            return null;
        }
        
    }

    public boolean contains(Object item) {
        
        Collection<Node> foundNodes = 
            parentNode.hasChildOfValue(nodeName, SimpleValue.build(item)); 
                        
        return (!foundNodes.isEmpty());
        
    }

    public boolean remove(Object item) {
            
        ObjectValue nodeValue = SimpleValue.build(item);
        
        try {
            
            Collection<Node> pointingNodes = 
                parentNode.hasChildOfValue(nodeName, nodeValue);

            if (pointingNodes.isEmpty()) {
                            
                return false;
                
            } else {
                
                for (Node pointingNode : pointingNodes) {
                    pointingNode.delete();
                }
    
                return true;
                
            }
            
        } catch (SBQLException e) {
            
            throw new NestedSBQLException(e);
            
        }
        
    }

}
