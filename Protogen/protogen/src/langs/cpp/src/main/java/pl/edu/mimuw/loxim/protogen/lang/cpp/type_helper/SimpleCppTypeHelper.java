package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class SimpleCppTypeHelper extends AbstractCppTypeHelper {
	private String typeName;
	
	
	@Override
	public String getTypeDeclaration() {
		return typeName;
	}
	
	public void setTypeName(String typeName) {
		this.typeName = typeName;
	}
	
	public boolean isMappable() {
		return true;
	}
	
	public String getMapType() {
		return getTypeDeclaration();	
	}

	public String isInCode(String value, String in) {
		return "(("+in+"&"+value+")=="+value+")";
	}
	
	public String getTypeName() {
		return typeName;
	}
	
	public void fieldInitialize(FieldType field, ProtocolTypeWrapper protocol) {
	}
	
	public String getIncludeCode() {
		return null;
	}
	
//	public List<String> getValues() {
//		return UniversalValuesSource.getInstance().getValuesForType(Type.fromValue(typeName));
//	}

}
