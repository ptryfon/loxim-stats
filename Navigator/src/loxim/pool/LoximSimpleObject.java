package loxim.pool;

public class LoximSimpleObject extends LoximObject {
	protected String value;
	
	public LoximSimpleObject(String value) {
		this.value = value;
	}
	
	public LoximSimpleObject(String name, String value) {
		this.name = name;
		this.value = value;
	}
	
	public String getValue() {
		return value;
	}
	
	public String toString2() {
		return value;
	}
}
