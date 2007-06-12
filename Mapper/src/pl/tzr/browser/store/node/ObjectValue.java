package pl.tzr.browser.store.node;

/**
 * Interface for possible values of the node
 * @author Tomasz Rosiek
 *
 */
public interface ObjectValue {

	void visit(ValueVisitor visitor);
	
}
