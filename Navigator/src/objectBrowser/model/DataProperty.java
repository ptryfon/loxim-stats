package objectBrowser.model;

public class DataProperty {
	private String name;
	private DataObject value;
	
	
	public DataProperty(String name, DataObject value) {
		super();
		this.name = name;
		this.value = value;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public DataObject getValue() {
		return value;
	}
	public void setValue(DataObject value) {
		this.value = value;
	}
}
