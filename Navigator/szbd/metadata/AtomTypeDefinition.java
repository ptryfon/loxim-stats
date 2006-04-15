package szbd.metadata;

public class AtomTypeDefinition extends TypeDefinition {
	DataType type;
	public AtomTypeDefinition(String _name, CardinalityType _card, DataType _type) {
		super(_name, _card);
		type = _type;
	}
	
	public String toString() {
		return "ATOM(name=" + name + ";card=" + card + ";type=" + type + ")";
	}
}
