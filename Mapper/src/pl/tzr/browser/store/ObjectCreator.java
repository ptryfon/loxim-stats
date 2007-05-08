package pl.tzr.browser.store;

import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.NodeImpl;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.browser.store.node.ValueVisitor;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.result.Result;
import pl.tzr.driver.loxim.result.ResultBag;
import pl.tzr.driver.loxim.result.ResultBinder;
import pl.tzr.driver.loxim.result.ResultBool;
import pl.tzr.driver.loxim.result.ResultInt;
import pl.tzr.driver.loxim.result.ResultReference;
import pl.tzr.driver.loxim.result.ResultString;

public class ObjectCreator implements ValueVisitor {
	
	final SimpleConnection connection;
	
	final LoximStore store;
	
	private Result createdObject;
	
	public ObjectCreator(SimpleConnection connection, LoximStore store) {
		this.connection = connection;
		this.store = store;
	}

	public void visitBoolValue(SimpleValue simpleValue) {
		createdObject = new ResultBool(simpleValue.getBoolean());
	}

	public void visitComplexValue(ComplexValue complexValue) {
		throw new UnsupportedOperationException();
	}

	public void visitIntValue(SimpleValue simpleValue) {
		createdObject = new ResultInt(simpleValue.getInteger());
	}

	public void visitStringValue(SimpleValue simpleValue) {
		createdObject = new ResultString(simpleValue.getString());
	}
	
	public void visitReferenceValue(ReferenceValue referenceValue) {
		createdObject = new ResultReference(referenceValue.getTargetNode().getReference());		
	}	
	
	public Node create(String objectName, ObjectValue value) throws SBQLException {
		
		value.visit(this);
		
		Result result = connection.executeParam("create ?", new ResultBinder(objectName, createdObject));
		
		if (!(result instanceof ResultBag)) throw new IllegalStateException();
		
		ResultBag resultBag = (ResultBag)result;
		
		Result item = resultBag.getItems().get(0);
		
		if (!(item instanceof ResultReference)) throw new IllegalStateException();
		
		return new NodeImpl(store, ((ResultReference)item).getRef(), objectName);
		
	}
	
	public void update(String ref, ObjectValue value) throws SBQLException {
		
		value.visit(this);
		
		connection.executeParam("? := ?", 
				new ResultReference(ref), createdObject);
		
	}



}
