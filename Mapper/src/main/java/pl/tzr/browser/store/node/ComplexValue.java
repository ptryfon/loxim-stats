package pl.tzr.browser.store.node;


/**
 * Class representing complex type of the node.
 * @author Tomasz Rosiek
 *
 */
public class ComplexValue implements ObjectValue {
	
	
	public void visit(ValueVisitor visitor) {
		visitor.visitComplexValue(this);		
	}
	
	@Override
    public String toString() {
		return "{COMPLEX}";
	}

	
}
