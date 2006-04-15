package szbd.metadata;

import java.util.LinkedList;
import java.util.List;

public class ComplexTypeDefinition extends TypeDefinition {
	List<TypeDefinition> childObjects;
	public ComplexTypeDefinition(String _name, CardinalityType _card) {
		super(_name, _card);
		childObjects = new LinkedList<TypeDefinition>();
	}
	
	public List<TypeDefinition> getItems() {
		return childObjects;
	}
	
	public void addItem(TypeDefinition td) {
		childObjects.add(td);
	}
	
	public String toString() {
		return "COMPLEX(name=" + name + ";card=" + card + ")";
	}
	
}
