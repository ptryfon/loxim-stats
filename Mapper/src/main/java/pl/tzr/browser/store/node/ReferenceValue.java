package pl.tzr.browser.store.node;

public class ReferenceValue implements ObjectValue {
	
	private final Node targetNode;
	
	public ReferenceValue(final Node targetNode) {
		this.targetNode = targetNode;
	}

	public void visit(ValueVisitor visitor) {
		visitor.visitReferenceValue(this);
	}

	public Node getTargetNode() {
		return targetNode;
	}
	
	@Override
	public String toString() {
		return targetNode.getName() + "@" + targetNode.getReference();
	}	

}
