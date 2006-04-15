package szbd.metadata;

public class TypeDefinition {
	String name;
	CardinalityType card;
	public TypeDefinition(String _name, CardinalityType _card) {
		name = _name;
		card = _card;
	}
	
	public String getName() {
		return name;
	}
	
	public CardinalityType getCard() {
		return card;
	}
}
