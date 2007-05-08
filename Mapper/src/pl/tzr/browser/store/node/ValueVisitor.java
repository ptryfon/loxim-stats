package pl.tzr.browser.store.node;

public interface ValueVisitor {

	void visitBoolValue(SimpleValue simpleValue);
	
	void visitIntValue(SimpleValue simpleValue);
	
	void visitStringValue(SimpleValue simpleValue);
	
	void visitComplexValue(ComplexValue complexValue);
	
	void visitReferenceValue(ReferenceValue referenceValue);
	
}
