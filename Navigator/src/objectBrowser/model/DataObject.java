package objectBrowser.model;

import loxim.pool.LoximObject;

import org.eclipse.draw2d.geometry.Point;

public abstract class DataObject extends DiagramNode {
	private String name;
	protected LoximObject target;
	
	public DataObject(String name, LoximObject target) {
		super();
		this.target = target;
		setLocation(new Point(30, 30));
		setName(name);
		
	}	
	
	public String getName() {
		return name;
	}
	
	public void setName(String name) {
		if (name == null) {
			if (target.getRef() != null) {
				this.name = "@" + target.getRef();
			} else {
				this.name = "<unnamed>";
			}
		} else {
			if (target.getRef() != null) {
				this.name = name + "(@" + target.getRef() + ")";
			} else {
				this.name = name;
			}
		}
	}
			
	public LoximObject getTarget() {
		return target;
	}
}
