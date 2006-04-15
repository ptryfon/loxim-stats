package szbd.metadata;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import szbd.SBQLException;
import szbd.client.Connection;
import szbd.client.result.Result;
import szbd.client.result.ResultBag;
import szbd.client.result.ResultString;

public class MetadataFactory {
	Connection con;
	
	public MetadataFactory(Connection _con) {
		con = _con;
	}
	public TypeDefinition fetchType(String path) throws SBQLException, IOException {
		CardinalityType card = CardinalityType.ONE_TO_ONE;
		TypeKind kind;
		String name;
		
		/* Pobieramy nazwe */		
		{
		Result resName = con.execute("deref((" + path + ").name);");
		if (!(resName instanceof ResultBag)) throw new SBQLException(-1, "Incorrect result type. Expected BAG.");
		if (((ResultBag)resName).getItems().size() < 1) throw new SBQLException(-1, "Incorrect result type. Expected some items in BAG.");
		if (!(((ResultBag)resName).getItems().get(0) instanceof ResultString)) throw new SBQLException(-2, "Incorrect result type. Expected STRING.");
		name = ((ResultString)((ResultBag)resName).getItems().get(0)).getValue();
		}		
		/* Pobieramy liczebno¶æ */
		{
		Result resCard = con.execute("deref((" + path + ").card);");
		if (!(resCard instanceof ResultBag)) throw new SBQLException(-1, "Incorrect result type. Expected BAG.");
		if (((ResultBag)resCard).getItems().size() >= 1) {
			if (!(((ResultBag)resCard).getItems().get(0) instanceof ResultString)) throw new SBQLException(-2, "Incorrect result type. Expected STRING.");
			String strCard = ((ResultString)((ResultBag)resCard).getItems().get(0)).getValue();
			if ("0..1".equals(strCard)) card = CardinalityType.ZERO_TO_ONE;
			else if ("1..1".equals(strCard)) card = CardinalityType.ONE_TO_ONE;
			else if ("0..*".equals(strCard)) card = CardinalityType.ZERO_TO_MANY;
			else if ("1..*".equals(strCard)) card = CardinalityType.ONE_TO_MANY;
			else throw new SBQLException(-3, "Invalid cardinality definition (" + strCard + ")");			
		}

		}
		/* Pobieramy rodzaj */
		{
		Result resKind = con.execute("deref((" + path + ").kind);");
		if (!(resKind instanceof ResultBag)) throw new SBQLException(-1, "Incorrect result type. Expected BAG.");
		if (((ResultBag)resKind).getItems().size() < 1) throw new SBQLException(-1, "Incorrect result type. Expected some items in BAG.");
		if (!(((ResultBag)resKind).getItems().get(0) instanceof ResultString)) throw new SBQLException(-2, "Incorrect result type. Expected STRING.");
		String strKind = ((ResultString)((ResultBag)resKind).getItems().get(0)).getValue();
		if ("complex".equals(strKind)) kind = TypeKind.COMPLEX;
		else if ("atomic".equals(strKind)) kind = TypeKind.ATOMIC;
		else if ("link".endsWith(strKind)) kind = TypeKind.LINK;
		else throw new SBQLException(-3, "Invalid type kind definition (" + strKind + ")");
		}
		
		switch (kind) {
			case ATOMIC:
				DataType type;
				Result resType = con.execute("deref((" + path + ").type);");
				if (!(resType instanceof ResultBag)) throw new SBQLException(-1, "Incorrect result type. Expected BAG.");
				if (((ResultBag)resType).getItems().size() < 1) throw new SBQLException(-1, "Incorrect result type. Expected some items in BAG.");
				if (!(((ResultBag)resType).getItems().get(0) instanceof ResultString)) throw new SBQLException(-2, "Incorrect result type. Expected STRING.");
				String strType = ((ResultString)((ResultBag)resType).getItems().get(0)).getValue();
				if ("string".equals(strType)) type = DataType.STRING;
				else if ("int".equals(strType)) type = DataType.INT;
				else if ("double".endsWith(strType)) type = DataType.DOUBLE;
				else throw new SBQLException(-3, "Invalid data type definition (" + strType + ")");
				return new AtomTypeDefinition(name, card, type); 				
			case COMPLEX:
				ComplexTypeDefinition typeDef = new ComplexTypeDefinition(name, card);
				
				Result res = con.execute("deref(("+path+").subobject.name);");
				
				if (!(res instanceof ResultBag)) throw new SBQLException(-1, "Incorrect result type. Expected BAG.");
				for (Result r : ((ResultBag)res).getItems()) {
					if (!(r instanceof ResultString)) throw new SBQLException(-1, "Incorrect result type. Expected STRING.");
					typeDef.addItem(fetchType("(" + path + ").subobject where name=\""+ ((ResultString)r).getValue() + "\""));
				}		
				
				return typeDef;
			case LINK:
				/* TODO */
				return null;
			default:
				/* NIGDY NIE POWINNO NAST¡PIÆ */
				return null;
		}
				
	
	}
	public List<TypeDefinition> fetchTypes() throws SBQLException, IOException {
		List<TypeDefinition> l = new LinkedList<TypeDefinition>();
		Result res = con.execute("deref(__MDN__.name);");
				
		if (!(res instanceof ResultBag)) throw new SBQLException(-1, "Incorrect result type. Expected BAG, found " + res + ".");
		for (Result r : ((ResultBag)res).getItems()) {
			if (!(r instanceof ResultString)) throw new SBQLException(-1, "Incorrect result type. Expected STRING.");
			try {
			l.add(fetchType("__MDN__ where name=\""+ ((ResultString)r).getValue() + "\""));
			} catch (Exception e) {
				System.err.println("Skipped type :" + ((ResultString)r).getValue());
				e.printStackTrace();
			}
		}		
		return l;
	}
}
