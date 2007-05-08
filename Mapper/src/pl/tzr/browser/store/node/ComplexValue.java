package pl.tzr.browser.store.node;

import java.util.Set;

public class ComplexValue implements ObjectValue {
	
	private final String ref;
	
	private final Set<String> propertyNames;
	
	public ComplexValue(String ref, Set<String> propertyNames) {
		this.ref = ref;
		this.propertyNames = propertyNames;
	}

	public Set<String> getPropertyNames() {
		return propertyNames;
	}
	
	public String getRef() {
		return ref;
	}
	
	public void visit(ValueVisitor visitor) {
		visitor.visitComplexValue(this);
		
	}	
	
	public String toString() {
		StringBuffer buffer = new StringBuffer("{");
		boolean isFirst = true;
		for (String item : propertyNames) {
			if (isFirst) {
				isFirst = false;
			} else {
				buffer.append(", ");
			}
			buffer.append(item);
		}
		buffer.append("}@" + ref);
		return buffer.toString();
	}
}
