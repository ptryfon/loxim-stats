package loxim.pool;

import java.util.LinkedList;
import java.util.List;

public class LoximCompositeObject extends LoximObject {
	List<LoximObject> properties = new LinkedList<LoximObject>();
	public LoximCompositeObject() {
		
	}
	
	public LoximCompositeObject(String name) {
		this.name = name;
	}	
	
	public List<LoximObject> getProperties() {
		return properties;
	}
	
	public String toString2() {
		StringBuffer buf = new StringBuffer();
		buf.append("<");
		boolean isFirst = true;
		for (LoximObject item : getProperties()) {
			if (isFirst) {
				isFirst = false;
			} else {
				buf.append(", ");
			}
			buf.append(item);
		}
		buf.append(">");
		return buf.toString();
	}
}
