package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class EnumCppTypeHelper extends AbstractCppTypeHelper {
	private String enumName;
	private ProtocolTypeWrapper protocol_wrapper;
	private EnumTypeWrapper enum_wrapper;

	public void fieldInitialize(FieldType field, ProtocolTypeWrapper protocol) {
		enumName = field.getObjectRef();
		protocol_wrapper = protocol;
		enum_wrapper = protocol_wrapper.findEnumWrapper(enumName);
		if (enumName == null)
			throw new RuntimeException("Field " + enumName
					+ " of type enum requires object-ref.");
		if (protocol_wrapper == null)
			throw new RuntimeException("Field " + enumName
					+ " of type enum requires protocol_type_wrapper");
		if (enum_wrapper == null)
			throw new RuntimeException("Field " + enumName
					+ " of type enum references unexisting enum: " + enumName);
	}

	@Override
	public String getTypeDeclaration() {
		return "enum " + enum_wrapper.getClassName();
	}

	public String getMapType() {
		return null;
	}

	public String getTypeName() {
		return enum_wrapper.getClassName();
	}

	public String isInCode(String value, String string) {
		return null;
	}

	public boolean isMappable() {
		return false;
	}

	public String getInitValue() {
		return "";
	}

	public String getInitCode(String name) {
		return ""; //TODO: default
	}

	public String readAssignCode(String name) {
		try {
			return enum_wrapper.getFactoryClassName() + "::getEnumByValue("
					+ enumTypeWrapperToTypeHelper(enum_wrapper).readCode() + "," + name
					+ ");";
		} catch (GeneratorException e) {
			throw new RuntimeException("Cannot generate 'readCode' code:", e);
		}
	}
	


	public String writeCode(String name) {
		try {
			StringBuffer res = new StringBuffer("\n"
					+ enumTypeWrapperToTypeHelper(enum_wrapper).getTypeDeclaration()
					+ " tmp_" + name + ";\n");
			res.append(enum_wrapper.getFactoryClassName()
					+ "::getValueByEnum(" + name + ", tmp_" + name + ");\n");
			res.append("\n"
					+ enumTypeWrapperToTypeHelper(enum_wrapper).writeCode(
							"tmp_" + name));
			return res.toString();
		} catch (GeneratorException e) {
			throw new RuntimeException("Cannot generate 'readCode' code:", e);
		}
	}

	public String getIncludeCode() {
		return null;
	}

}
