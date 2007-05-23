package pl.tzr.browser.store;

import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Set;

import pl.tzr.browser.session.LoximSession;
import pl.tzr.browser.store.node.ComplexValue;
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
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.exception.NestedSBQLException;

/**
 * Implementation of Executor interface providing data manipulation operations
 * for LoXiM database 
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
		Result source = new ResultReference(child.getReference());
		
		executeQuery("?:<?", target, source);

	}
	
	public Node createObject(String name, ObjectValue value) {
		
		ObjectCreator objectCreator = new ObjectCreator(loximSession);
		Node proxy = objectCreator.create(name, value);
		return proxy;
	}

	public void deleteObject(String ref) {		
		executeQuery("delete ?", new ResultReference(ref));
	}

	public ObjectValue getValue(String ref) throws SBQLException {
				
		Result result = executeQuery("deref(?)", new ResultReference(ref));
		
		if (result instanceof ResultStruct) {
			return getComplexValue(ref, (ResultStruct)result);
		} else if (result instanceof ResultString) {
			return getSimpleValue((ResultString)result);
		} else if (result instanceof ResultInt) { 
			return getSimpleIntValue((ResultInt)result);
		} else if (result instanceof ResultBool) {
			return getSimpleBoolValue((ResultBool)result);
		} else if (result instanceof ResultReference) {
			return getReferenceValue((ResultReference)result);
		} else throw new InvalidDataStructureException();
	}
	
	private ObjectValue getReferenceValue(ResultReference resultReference) throws SBQLException {
		
		String query = "(? as i).(i as r, nameof(i) as n)";
		
		Result result = executeQuery(query, resultReference);
		
		Node node;
		
		try {
					
			Result firstItem = ((ResultBag)result).getItems().get(0); 				
			node = getNodeFromPairOfBinders((ResultStruct)firstItem);
		
		} catch (ClassCastException e) {
			throw new InvalidDataStructureException();
		} catch (ArrayIndexOutOfBoundsException e) {
			throw new InvalidDataStructureException();
		}
		
		return new ReferenceValue(node);
	}
	
	private ObjectValue getSimpleIntValue(ResultInt resultInteger) throws SBQLException {
		return new SimpleValue(resultInteger.getValue());
	}
	
	private ObjectValue getSimpleBoolValue(ResultBool resultBool) throws SBQLException {
		return new SimpleValue(resultBool.getValue());
	}
	
	private ObjectValue getSimpleValue(ResultString string) throws SBQLException { 
		return new SimpleValue(string.getValue());
	}
	
	private ObjectValue getComplexValue(String parentRef, ResultStruct struct) {
		
		Set<String> childNames = new HashSet<String>();
		
		try {
				
			for (Result item : struct.getItems()) {
				childNames.add(((ResultBinder)item).getKey());
			}	
		
		} catch (ClassCastException e) {
			throw new InvalidDataStructureException();
		}
		
		return new ComplexValue(parentRef, childNames);
	}

	public Collection<Node> getChildNodes(String parentRef, String propertyName) {
		
		Result childResult;
		
		childResult = executeQuery("?." + propertyName, new ResultReference(parentRef));
		
		
		Collection<Node> children = new LinkedList<Node>();
		
		try {
		
			ResultBag childBag = (ResultBag)childResult;
			for (Result item : childBag.getItems()) {
				ResultReference itemReference = (ResultReference)item;
				Node childProxy = loximSession.createNode(itemReference.getRef(), propertyName);
				children.add(childProxy);
			}	
		
		} catch (ClassCastException e) {
			throw new InvalidDataStructureException();
		}
		
		return children;
	}
	
	public void setValue(String ref, ObjectValue value) throws SBQLException {
		
		if (value instanceof SimpleValue) {
			
			ObjectCreator objectCreator = new ObjectCreator(loximSession);
			objectCreator.update(ref, value);
			
		} else throw new UnsupportedOperationException();
	}

	public Set<Node> executeQuery(String query) throws SBQLException {
		
		Set<Node> results = new HashSet<Node>();		
		
		String internalQuery = "(" + query + " as i).(i as r, nameof(i) as n)"; 
		
		Result result = loximSession.getConnection().execute(internalQuery);
		
		try {
					
			for (Result item : ((ResultBag)result).getItems()) {						
				
				Node newObject = getNodeFromPairOfBinders((ResultStruct)item);			
				results.add(newObject);
				
			}
		
		} catch (ClassCastException e) {			
			throw new InvalidDataStructureException();			
		}
		
		return results;
	}
	
	private Node getNodeFromPairOfBinders(ResultStruct itemStruct) {
		
		ResultReference refResult;
		ResultString nameResult;
		
		try {
			
			refResult = (ResultReference)((ResultBinder)itemStruct.getItems().get(0)).getValue();					
			nameResult = (ResultString)((ResultBinder)itemStruct.getItems().get(1)).getValue();
			
		
		} catch (ClassCastException e) {
			throw new InvalidDataStructureException();
		} catch (ArrayIndexOutOfBoundsException e) {
			throw new InvalidDataStructureException();
		}
		
		Node newObject = loximSession.createNode(refResult.getRef(), nameResult.getValue());
		
		return newObject;
	}
	
	private Result executeQuery(String query, Result... params) {
		try {
			return loximSession.getConnection().executeParam(query, params);
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}
	}

}
