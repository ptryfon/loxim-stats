package pl.tzr.browser.store;

import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import pl.tzr.browser.session.LoximSession;
import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.result.Result;
import pl.tzr.driver.loxim.result.ResultBag;
import pl.tzr.driver.loxim.result.ResultBinder;
import pl.tzr.driver.loxim.result.ResultBool;
import pl.tzr.driver.loxim.result.ResultInt;
import pl.tzr.driver.loxim.result.ResultReference;
import pl.tzr.driver.loxim.result.ResultString;
import pl.tzr.driver.loxim.result.ResultStruct;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.exception.NestedSBQLException;

/**
 * Implementation of Executor interface providing data manipulation operations
 * for LoXiM database
 * 
 * @author Tomasz Rosiek
 * 
 */
public class LoximExecutor implements Executor {

    private LoximSession loximSession;

    public LoximExecutor(LoximSession loximSession) {
        this.loximSession = loximSession;
    }

    public void addChild(String parentRef, Node child) {
        
        Result target = new ResultReference(parentRef);

        if (child.isDetached())
            attachObject(child);

        executeQuery("?:<?", target, new ResultReference(child.getReference()));
        
    }
    
    public Node loadObject(String ref) {
        return getReferenceValue(new ResultReference(ref)).getTargetNode();
    }

    public Node createSimpleObject(String name, ObjectValue value) {

        Node node = new LoximNode(name, value);

        attachObject(node);

        return node;
    }

    public void attachObject(Node node) {
        
        try {        

            if (!node.isDetached())
                throw new IllegalStateException(
                        "Cannot persist already persistent node");
    
            if (node.getValue() instanceof ComplexValue) {
                
                // FIXME: LoXiM doesn't allow to create empty composite object
                ResultBinder b = new ResultBinder("bugfix", new ResultInt(0));
                ResultStruct s = new ResultStruct(b);
    
                ResultBag bag = (ResultBag)
                    loximSession.getConnection().executeParam(
                            "create ? as " + node.getName(), s);
    
                ResultReference ref = (ResultReference) (bag.getItems().get(0));    
    
                Collection<Node> childNodes = node.getAllChildNodes();
    
                node.markAttached(loximSession, ref.getRef());
    
                for (Node childNode : childNodes) {
                    
                    attachObject(childNode);
    
                    loximSession.getConnection().executeParam("? :< ?",
                            new ResultReference(node.getReference()),
                            new ResultReference(childNode.getReference()));
    
                }
    
            } else if (node.getValue() instanceof SimpleValue
                    || node.getValue() instanceof ReferenceValue) {
    
                ObjectCreator objectCreator = new ObjectCreator(loximSession);
                String ref = objectCreator.create(node.getName(), node.getValue());
                node.markAttached(loximSession, ref);
    
            } else
                throw new IllegalStateException();
        
        } catch (SBQLException e) {
            throw new NestedSBQLException(e);
        } 
        

    }

    public void deleteObject(String ref) {
        executeQuery("delete ?", new ResultReference(ref));
    }

    public ObjectValue getValue(String ref) {

        Result result = executeQuery("deref(?)", new ResultReference(ref));

        if (result instanceof ResultStruct) {
            return getComplexValue(ref, (ResultStruct) result);
        } else if (result instanceof ResultString || 
                result instanceof ResultInt || 
                result instanceof ResultBool) {
            return getSimpleValue(result);
        } else if (result instanceof ResultReference) {
            return getReferenceValue((ResultReference) result);
        } else if (result instanceof ResultBag
                && ((ResultBag) result).getItems().isEmpty()) {
            throw new DeletedException(ref);
        } else
            throw new InvalidDataStructureException();
    }

    private ReferenceValue getReferenceValue(ResultReference resultReference) {

        String query = "(? as i).(i as r, nameof(i) as n)";

        Result result = executeQuery(query, resultReference);

        try {
            
            if (((ResultBag) result).getItems().isEmpty()) 
                throw new DeletedException(resultReference.getRef());
 
            Result firstItem = ((ResultBag) result).getItems().get(0);
            Node node = getNodeFromPairOfBinders((ResultStruct) firstItem);

            return new ReferenceValue(node);           

        } catch (ClassCastException e) {
            throw new InvalidDataStructureException();
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new InvalidDataStructureException();
        }

        
    }
    
    private SimpleValue getSimpleValue(Result result) {
        if (result instanceof ResultString) {
            return getSimpleValue((ResultString) result);
        } else if (result instanceof ResultInt) {
            return getSimpleIntValue((ResultInt) result);
        } else if (result instanceof ResultBool) {
            return getSimpleBoolValue((ResultBool) result);
        } else throw new IllegalStateException();
    }

    private SimpleValue getSimpleIntValue(ResultInt resultInteger) {
        return new SimpleValue(resultInteger.getValue());
    }

    private SimpleValue getSimpleBoolValue(ResultBool resultBool) {
        return new SimpleValue(resultBool.getValue());
    }

    private SimpleValue getSimpleValue(ResultString string) {
        return new SimpleValue(string.getValue());
    }

    private ObjectValue getComplexValue(String parentRef, ResultStruct struct) {

        return new ComplexValue();

    }

    public Collection<Node> getChildNodes(
            String parentRef, String propertyName) {

        Result childResult;

        childResult = executeQuery("?." + propertyName, new ResultReference(
                parentRef));

        Collection<Node> children = new LinkedList<Node>();

        try {

            ResultBag childBag = (ResultBag) childResult;
            for (Result item : childBag.getItems()) {
                ResultReference itemReference = (ResultReference) item;
                Node childProxy = loximSession.createNode(itemReference
                        .getRef(), propertyName);
                children.add(childProxy);
            }

        } catch (ClassCastException e) {
            throw new InvalidDataStructureException();
        }

        return children;
    }

    public void setValue(String ref, ObjectValue value) {

        if (value instanceof SimpleValue || value instanceof ReferenceValue) {

            ObjectCreator objectCreator = new ObjectCreator(loximSession);
            objectCreator.update(ref, value);        
        } else
            throw new UnsupportedOperationException();
    }
    
    

    public Collection<Node> find(String query, ObjectValue... objectValues) 
        throws SBQLException {

        List<Node> results = new LinkedList<Node>();
        
        Result[] paramResults = buildResultsFromValues(objectValues);

        String internalQuery = "(" + query + " as i).(i as r, nameof(i) as n)";

        Result result = loximSession.getConnection().executeParam(
                internalQuery, paramResults);

        try {

            for (Result item : ((ResultBag) result).getItems()) {

                Node newObject = getNodeFromPairOfBinders((ResultStruct) item);
                results.add(newObject);

            }

        } catch (ClassCastException e) {
            throw new InvalidDataStructureException();
        }

        return results;
    }
    
    public Collection<SimpleValue> findPrimitive(
            String query, 
            ObjectValue... objectValues) 
        throws SBQLException {
    
        List<SimpleValue> results = new LinkedList<SimpleValue>();
        
        Result[] paramResults = buildResultsFromValues(objectValues);            
    
        Result result = loximSession.getConnection().executeParam(
                query, paramResults);
                    
        try {
            
            if (result instanceof ResultBag) {
    
            for (Result item : ((ResultBag) result).getItems()) {
                
                SimpleValue simpleValue = getSimpleValue(item); 
    
                results.add(simpleValue);
    
            }
            
            } else {
                
                SimpleValue simpleValue = getSimpleValue(result); 
                
                results.add(simpleValue);
                
            }
    
        } catch (ClassCastException e) {
            throw new InvalidDataStructureException();
        }
    
        return results;
    }    
    
    private Result[] buildResultsFromValues(ObjectValue[] values) {
    
    Result[] results = new Result[values.length];
    
    int i = 0;
    
    for (ObjectValue value : values) {
    
        if (value instanceof SimpleValue || value instanceof ReferenceValue) {

            ObjectCreator objectCreator = new ObjectCreator(loximSession);
            results[i] = objectCreator.createResult(value);

        } else throw new UnsupportedOperationException();
    
        i++;
    
    }
    
    return results;
}

    private Node getNodeFromPairOfBinders(ResultStruct itemStruct) {

        ResultReference refResult;
        ResultString nameResult;

        try {

            refResult = (ResultReference) ((ResultBinder) itemStruct.getItems()
                    .get(0)).getValue();
            nameResult = (ResultString) ((ResultBinder) itemStruct.getItems()
                    .get(1)).getValue();

        } catch (ClassCastException e) {
            throw new InvalidDataStructureException();
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new InvalidDataStructureException();
        }

        Node newObject = loximSession.createNode(refResult.getRef(), nameResult
                .getValue());

        return newObject;
    }
    
    public boolean isChild(String parentRef, Node childNode) {

        Result result = executeQuery("?." + childNode.getName()
                + " intersect ?",
                new ResultReference(parentRef),
                new ResultReference(childNode.getReference()));

        try {
            return !((ResultBag) result).getItems().isEmpty();
        } catch (ClassCastException e) {
            throw new InvalidDataStructureException();
        }

    }


    
    public int childAmount(String parentRef, String childName) {
        
        Result result = executeQuery("count(?." + childName + ")",
                new ResultReference(parentRef));

        try {
            return ((ResultInt) result).getValue();
        } catch (ClassCastException e) {
            throw new InvalidDataStructureException();
        }
        
        
    }
    
    public void removeAllChildren(String parentRef, String childName) {
        
        executeQuery("delete ?." + childName,
                new ResultReference(parentRef));
                
    }   
    
    public Set<Node> findChildrenOfValue(
            String parentRef, String name, ObjectValue value) {
        
        ObjectCreator objectCreator = new ObjectCreator(loximSession);
        Result valueResult = objectCreator.createResult(value);
        
        Result result = executeQuery("(?." + name + " as tmp).(tmp as ptr, deref(tmp) intersect ? as cont).ptr",
                new ResultReference(parentRef),
                valueResult);

        Set<Node> results = new HashSet<Node>();
        
        try {

            for (Result item : ((ResultBag) result).getItems()) {
                
                ResultReference refItem = (ResultReference)item;
                
                Node newObject = loximSession.createNode(refItem.getRef(), name);
                results.add(newObject);

            }

        } catch (ClassCastException e) {
            throw new InvalidDataStructureException();
        }

        return results;
        
    }

    private Result executeQuery(String query, Result... params) {
        try {
            return loximSession.getConnection().executeParam(query, params);
        } catch (SBQLException e) {
            throw new NestedSBQLException(e);
        }
    }

    

}
