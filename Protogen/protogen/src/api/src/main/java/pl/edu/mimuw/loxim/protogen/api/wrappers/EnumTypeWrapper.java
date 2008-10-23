package pl.edu.mimuw.loxim.protogen.api.wrappers;

import java.util.LinkedList;
import java.util.List;

import pl.edu.mimuw.loxim.protogen.api.CodeHelper;
import pl.edu.mimuw.loxim.protogen.api.jaxb.EnumType;

public class EnumTypeWrapper {
	public static final String ALL_CONSTANT = "ALL";
	public static final String NONE_CONSTANT = "NONE";
	public static final String ODD_CONSTANT = "ODD";
	private static List<String> ENUMMAPVALUESLIST;
	private EnumType enum_type;
	private ProtocolTypeWrapper protocol;

	public EnumTypeWrapper(EnumType enum_type, ProtocolTypeWrapper a_protocol) {
		super();
		this.enum_type = enum_type;
		this.protocol = a_protocol;
	}

	public String getClassName() {
		return CodeHelper.upperFirstChar(enum_type.getName()) + "Enum";
	}

	public String getFactoryClassName() {
		return CodeHelper.upperFirstChar(enum_type.getName()) + "Factory";
	}

	public String getMapClassName() {
		return CodeHelper.upperFirstChar(enum_type.getName()) + "Map";
	}

	public EnumType getEnum_type() {
		return enum_type;
	}

	public void setEnum_type(EnumType enum_type) {
		this.enum_type = enum_type;
	}

	public ProtocolTypeWrapper getProtocol() {
		return protocol;
	}

	public void setProtocol(ProtocolTypeWrapper protocol) {
		this.protocol = protocol;
	}

	public List<String> getExampleValues() {
		List<String> res = new LinkedList<String>();
		int size = enum_type.getItem().size();
		if (size > 0)
			res.add(enum_type.getItem().get(0).getName());
		if (size > 1)
			res.add(enum_type.getItem().get(size - 1).getName());
		if (size > 2)
			res.add(enum_type.getItem().get(size / 2).getName());
		return res;

	}

	public List<?> getEnumMapValuesList() {
		if (ENUMMAPVALUESLIST == null) {
			ENUMMAPVALUESLIST = new LinkedList<String>();
			ENUMMAPVALUESLIST.add(ALL_CONSTANT);
			ENUMMAPVALUESLIST.add(NONE_CONSTANT);
			ENUMMAPVALUESLIST.add(ODD_CONSTANT);
		}
		return ENUMMAPVALUESLIST;
	}

	// public CppTypeHelper getCppAsTypeHelper() throws GeneratorException
	// {
	// try {
	// return CppTypeHelperFactory.getCppTypeHelper(enum_type.getAsType());
	// } catch (PlexusContainerException e) {
	// throw new GeneratorException(e);
	// } catch (ComponentLookupException e) {
	// throw new GeneratorException(e);
	// }
	// }
}
