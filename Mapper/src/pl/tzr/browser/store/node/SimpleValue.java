package pl.tzr.browser.store.node;

public class SimpleValue implements ObjectValue {
	
	public static enum Type {BOOL, INT, STRING};
		
	private final Object value;
	
	private final Type type;
	
	public SimpleValue(boolean value) { 
		this.value = value;
		this.type = Type.BOOL;
	}
	
	public SimpleValue(int value) {
		this.value = value;
		this.type = Type.INT;
	}
	
	public SimpleValue(String value) {
		this.value = value;
		this.type = Type.STRING;
	}
	
	public void visit(ValueVisitor visitor) {
		switch (type) {
		case BOOL:
			visitor.visitBoolValue(this);
			break;
		case INT:
			visitor.visitIntValue(this);
			break;
		case STRING:
			visitor.visitStringValue(this);
			break;
		}
		
	}
	
		
	public Boolean getBoolean() {
		return (Boolean)value;
	}
	
	public String getString() {
		return (String)value;
	}
	
	public Integer getInteger() {
		return (Integer)value;
	}
	
	public Type getType() {
		return type;
	}	
		
	public String toString() {
		if (type == Type.STRING) {
			return "\"" + value + "\"";
		} else {
			return value.toString();
		}
	}

}