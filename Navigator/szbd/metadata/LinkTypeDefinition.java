package szbd.metadata;

import java.util.LinkedList;

public class LinkTypeDefinition extends TypeDefinition {
	TypeDefinition target;
	
	public LinkTypeDefinition(String _name, CardinalityType _card) {
		super(_name, _card);
		target = null;
	}	
	
	protected void setTarget(TypeDefinition _target) {
		target = _target;
	}
	
	public TypeDefinition getTarget() {
		return target;
	}
	
	public String toString() {
		return "LINK(name=" + name + ";card=" + card + ")";
	}
	
}
