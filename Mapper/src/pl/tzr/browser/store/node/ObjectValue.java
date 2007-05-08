package pl.tzr.browser.store.node;

public interface ObjectValue {

	void visit(ValueVisitor visitor);
	
}
