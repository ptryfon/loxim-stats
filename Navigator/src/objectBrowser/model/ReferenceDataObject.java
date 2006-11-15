package objectBrowser.model;

import loxim.pool.LoximReference;

public class ReferenceDataObject extends DataObject {
	
	public ReferenceDataObject(LoximReference object) {
		super(object.getName(), object);
	}

}
