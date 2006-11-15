package objectBrowser.model;

import loxim.pool.LoximSimpleObject;

public class SimpleDataObject extends DataObject {	
	
	public String getValue() {
		return ((LoximSimpleObject)target).getValue();
	}


	public SimpleDataObject(LoximSimpleObject object) {
		super(object.getName(), object);
	}
	
	
}
