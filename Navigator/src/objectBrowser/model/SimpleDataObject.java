package objectBrowser.model;

public class SimpleDataObject extends DataObject {	
	protected String value;
	
	public String getValue() {
		return value;
	}

	public void setValue(String value) {
		this.value = value;
	}

	public SimpleDataObject(String name, String value) {
		super(name);
		this.value = value;
	}
	
	
}
