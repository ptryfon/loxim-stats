package pl.tzr.browser.store;

import pl.tzr.browser.session.LoximSession;
import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.browser.store.node.ValueVisitor;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.result.Result;
import pl.tzr.driver.loxim.result.ResultBag;
import pl.tzr.driver.loxim.result.ResultBinder;
import pl.tzr.driver.loxim.result.ResultBool;
import pl.tzr.driver.loxim.result.ResultInt;
import pl.tzr.driver.loxim.result.ResultReference;
import pl.tzr.driver.loxim.result.ResultString;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.exception.NestedSBQLException;

public class ObjectCreator implements ValueVisitor {
	
	final LoximSession loximSession;
	
	private Result createdObject;
	
	public ObjectCreator(LoximSession loximSession) {
		this.loximSession = loximSession;
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
		if (referenceValue.getTargetNode().isDetached()) 
			throw new IllegalStateException("Tried to persist reference to detached object");
		createdObject = new ResultReference(referenceValue.getTargetNode().getReference());		
	}	
	
	public String create(String objectName, ObjectValue value) {
		
		value.visit(this);
		
		Result result;
		
		try {
		
		result = loximSession.getConnection().executeParam(
				"create ?", new ResultBinder(objectName, createdObject));
		
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}
		
		try {
		
			Result item = ((ResultBag)result).getItems().get(0);
					
			return ((ResultReference)item).getRef();
		
		} catch (ClassCastException e) {
			throw new InvalidDataStructureException();
		} catch (ArrayIndexOutOfBoundsException e) {
			throw new InvalidDataStructureException();
		}
		

		
	}
	
	public void update(String ref, ObjectValue value) {
		
		value.visit(this);
		
		try {
		
		loximSession.getConnection().executeParam("? := ?", 
				new ResultReference(ref), createdObject);
		
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}
		
	}



}
