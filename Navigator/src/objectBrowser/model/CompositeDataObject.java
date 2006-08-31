package objectBrowser.model;

import java.util.Collection;
import java.util.LinkedList;
import java.util.List;

public class CompositeDataObject extends DataObject {
	private List<DataObject> children = new LinkedList<DataObject>();
	public CompositeDataObject(String name) {
		super(name);
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
	
	public CompositeDataObject(String name, Collection<DataObject> children) {
		super(name);
		this.children.addAll(children);
	}
	
	public List<DataObject>getChildren() {
		return children;
	}
}
