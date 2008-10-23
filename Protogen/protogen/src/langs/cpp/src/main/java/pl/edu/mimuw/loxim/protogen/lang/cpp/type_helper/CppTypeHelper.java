package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public interface CppTypeHelper {
	public static String ROLE=CppTypeHelper.class.getName();
	/*
	 * Umożliwia inicjalizajcę pola na podstawie dodatkowych zmiennych.
	 */
	public void fieldInitialize(FieldType field, ProtocolTypeWrapper protocol);
	
	public String getTypeName();
	public String getTypeDeclaration();
	
	public String getInitValue();
	public String getInitCode(String name);
	
	public String getFieldDefinition(String name);	
	public String getSetter(String name);
	public String getGetter(String name);
	//public String getDefineConstructor(String value);
	
	public String destroyCode(String name);
	
	public String readAssignCode(String name);
//	public String writeAssignCode(String name);
	
	public String readCode();
	public String writeCode(String value);
	
	public String equalsCode(String name);
	public String compareCode(String val1,String val2);
	
	public boolean isMappable();

	public String getMapType();

	public String isInCode(String value, String string);

	public String getIncludeCode();
	
	//public List<String> getValues();
}
