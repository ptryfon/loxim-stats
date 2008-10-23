package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import org.codehaus.plexus.PlexusContainerException;
import org.codehaus.plexus.component.repository.exception.ComponentLookupException;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class EnumMapCppTypeHelper extends AbstractCppTypeHelper {
	private String enumName;
	private ProtocolTypeWrapper protocol_wrapper;
	private EnumTypeWrapper enum_wrapper;
	
	public void fieldInitialize(FieldType field, ProtocolTypeWrapper protocol) {
		enumName=field.getObjectRef();
		protocol_wrapper=protocol;
		enum_wrapper=protocol_wrapper.findEnumWrapper(enumName);
		if (enumName==null)
			throw new RuntimeException("Field of type enum-map requires object-ref.");
		if (protocol_wrapper==null)
			throw new RuntimeException("Field of type enum-map requires protocol_type_wrapper");
		if (enum_wrapper==null)
			throw new RuntimeException("Field of type enum-map references unexisting enum: "+enumName);
	}
	
	public static CppTypeHelper enumTypeWrapperToTypeHelper(EnumTypeWrapper enum_wrapper) throws GeneratorException
	{
		try {
			return CppTypeHelperFactory.getCppTypeHelper(enum_wrapper.getEnum_type().getAsType());
		} catch (PlexusContainerException e) {
			throw new GeneratorException(e);
		} catch (ComponentLookupException e) {
			throw new GeneratorException(e);
		}
	}
	
	@Override
	public String getTypeDeclaration() {
		return "struct "+enum_wrapper.getMapClassName();
	}



	public String getMapType() {
		// TODO Auto-generated method stub
		return null;
	}

	public String getTypeName() {
		return enum_wrapper.getMapClassName();
	}

	
	public String isInCode(String value, String string) {
		return null;
	}

	public boolean isMappable() {
		return false;
	}
	
	public String getInitValue()
	{
		return "";
	}
	
	public String getInitCode(String name)
	{
		return "memset(&"+name+",0,sizeof("+name+"));";
	}
	
	
	public String readAssignCode(String name)
	{
		try {
			return enum_wrapper.getFactoryClassName()+"::getEnumMapByValue("+enumTypeWrapperToTypeHelper(enum_wrapper).readCode()+","+name+");";
		} catch (GeneratorException e) {
			throw new RuntimeException("Cannot generate 'readCode' code:",e);
		}
	}
	
	public String writeCode(String name)
	{
		try 
		{
			StringBuffer res=new StringBuffer("\n"+enumTypeWrapperToTypeHelper(enum_wrapper).getTypeDeclaration()+" tmp_"+name+";\n");
			res.append(enum_wrapper.getFactoryClassName()+"::getValueByEnumMap("+name+", tmp_"+name+");\n");
			res.append("\n"+enumTypeWrapperToTypeHelper(enum_wrapper).writeCode("tmp_"+name));
			return res.toString();
		} catch (GeneratorException e) {
			throw new RuntimeException("Cannot generate 'readCode' code:",e);
		}
	}
	

	public String compareCode(String val1,String val2)
	{
		return "("+enum_wrapper.getFactoryClassName()+"::mapEquals("+val1+","+val2+")"+")";
	}
	
	public String getIncludeCode() {
		return null;
	}

}
