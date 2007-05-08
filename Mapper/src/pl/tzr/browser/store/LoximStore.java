package pl.tzr.browser.store;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;

import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.NodeImpl;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.result.Result;
import pl.tzr.driver.loxim.result.ResultBag;
import pl.tzr.driver.loxim.result.ResultBinder;
import pl.tzr.driver.loxim.result.ResultBool;
import pl.tzr.driver.loxim.result.ResultInt;
import pl.tzr.driver.loxim.result.ResultReference;
import pl.tzr.driver.loxim.result.ResultString;
import pl.tzr.driver.loxim.result.ResultStruct;


public class LoximStore implements Store {
	
	private SimpleConnection connection;
	
	private Map<String, Node> fetchedNodes = new HashMap<String, Node>();
	
	public LoximStore(SimpleConnection connection) {
		this.connection = connection;
	}

	public void addChild(String parentRef, Node child) throws SBQLException {
		
		Result target = new ResultReference(parentRef);
		Result source = new ResultReference(child.getReference());
		
		connection.executeParam("?:<?", target, source);

	}
	
	public Node createObject(String name, ObjectValue value) throws SBQLException {
		
		ObjectCreator objectCreator = new ObjectCreator(connection, this);
		Node proxy = objectCreator.create(name, value);
		return proxy;
	}

	public void deleteObject(String ref) throws SBQLException {
		connection.executeParam("delete ?", new ResultReference(ref));
	}

	public ObjectValue getValue(String ref) throws SBQLException {
				
		Result result = connection.executeParam("deref(?)", new ResultReference(ref));
		
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
		} else throw new IllegalStateException();
	}
	
	private ObjectValue getReferenceValue(ResultReference resultReference) throws SBQLException {
		
		String query = "(? as i).(i as r, nameof(i) as n)";
		
		Result result = connection.executeParam(query, resultReference);
		
		if (!(result instanceof ResultBag)) throw new IllegalStateException();
		
		if (((ResultBag)result).getItems().size() > 1) throw new IllegalStateException();
		
		//Czy napewno? Czy referencja nie może byc pusta?
		if (((ResultBag)result).getItems().size() == 0) throw new IllegalStateException();
		
		Result firstItem = ((ResultBag)result).getItems().get(0); 
			
		Node node = getNodeFromPairOfBinders((ResultStruct)firstItem);	 
		
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
	
	private ObjectValue getComplexValue(String parentRef, ResultStruct struct) throws SBQLException {
		Set<String> childNames = new HashSet<String>();
		for (Result item : struct.getItems()) {
			if (!(item instanceof ResultBinder)) throw new IllegalStateException();
			ResultBinder binder = (ResultBinder)item;
			childNames.add(binder.getKey());
		}			
		return new ComplexValue(parentRef, childNames);
	}

	public Collection<Node> getChildNodes(String parentRef, String propertyName) throws SBQLException {
		
		Result childResult = connection.executeParam("?." + propertyName, 
				new ResultReference(parentRef));
		
		Collection<Node> children = new LinkedList<Node>();
		
		if (!(childResult instanceof ResultBag)) throw new IllegalStateException();
		ResultBag childBag = (ResultBag)childResult;
		for (Result item : childBag.getItems()) {
			if (!(item instanceof ResultReference)) throw new IllegalStateException();
			ResultReference itemReference = (ResultReference)item;
			Node childProxy = createNode(itemReference.getRef(), propertyName);
			children.add(childProxy);
		}	
		return children;
	}
	
	public void setValue(String ref, ObjectValue value) throws SBQLException {
		
		if (value instanceof SimpleValue) {
			
			ObjectCreator objectCreator = new ObjectCreator(connection, this);
			objectCreator.update(ref, value);
			
		} else throw new UnsupportedOperationException();
	}

	public Set<Node> executeQuery(String query) throws SBQLException {
		
		Set<Node> results = new HashSet<Node>();		
		
		String internalQuery = "(" + query + " as i).(i as r, nameof(i) as n)"; 
		
		Result result = connection.execute(internalQuery);
		
		if (!(result instanceof ResultBag)) throw new IllegalStateException();
		
		for (Result item : ((ResultBag)result).getItems()) {
			
			if (!(item instanceof ResultStruct)) throw new IllegalStateException();				
			
			Node newObject = getNodeFromPairOfBinders((ResultStruct)item);
			
			results.add(newObject);
		}
		
		
		
		return results;
	}
	
	private Node getNodeFromPairOfBinders(ResultStruct itemStruct) {
		if (itemStruct.getItems().size() != 2) throw new IllegalStateException();
		
		if (!(itemStruct.getItems().get(0) instanceof ResultBinder))
				throw new IllegalStateException();
		
		if (!(itemStruct.getItems().get(1) instanceof ResultBinder))
				throw new IllegalStateException();
	
		Result refResult = ((ResultBinder)itemStruct.getItems().get(0)).getValue();
		
		if (!(refResult instanceof ResultReference)) throw new IllegalStateException();
		
		Result nameResult = ((ResultBinder)itemStruct.getItems().get(1)).getValue();
		
		if (!(nameResult instanceof ResultString)) throw new IllegalStateException();		
		
		Node newObject = createNode( 
				((ResultReference)refResult).getRef(),
				((ResultString)nameResult).getValue()
				);
		
		return newObject;
	}
	
	private Node createNode(String ref, String name) {
				
		if (fetchedNodes.containsKey(ref)) {
			Node node = fetchedNodes.get(ref);
			return node;
		} else {
			Node node = new NodeImpl(this, ref, name);
			fetchedNodes.put(ref, node);
			return  node;
		}				
		
	}

}
