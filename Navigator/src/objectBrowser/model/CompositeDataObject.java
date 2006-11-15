package objectBrowser.model;

import java.util.LinkedList;
import java.util.List;

import loxim.pool.LoximCompositeObject;

public class CompositeDataObject extends DataObject {
	
	private List<DataObject> children = new LinkedList<DataObject>();
	
	public CompositeDataObject(LoximCompositeObject object) {
		super(object.getName(), object);
	}
	
	public void addChild(DataObject child){
		addChild(child, -1);
	}

	public void addChild(DataObject child, int index){
		if (index >= 0)
			children.add(index,child);
		else
			children.add(child);
		fireChildAdded(PROP_CHILDREN, child, new Integer(index));
	}	
	
	
	public List<DataObject>getChildren() {
		return children;
	}
}
