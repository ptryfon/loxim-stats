package objectBrowser.model;

import java.util.LinkedList;
import java.util.List;

import org.eclipse.draw2d.geometry.Point;

public abstract class DataObject extends DiagramNode {
	private String name;
	
	public DataObject(String name) {
		super();
		setLocation(new Point(30, 30));
		this.name = name;
	}	
	
	public String getName() {
		return name;
	}
	
	public void setName(String name) {
		this.name = name;
	}
			
}
