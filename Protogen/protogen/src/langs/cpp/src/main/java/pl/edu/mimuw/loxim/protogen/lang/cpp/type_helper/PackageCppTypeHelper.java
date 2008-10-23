package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import org.codehaus.plexus.PlexusContainerException;
import org.codehaus.plexus.component.repository.exception.ComponentLookupException;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class PackageCppTypeHelper extends AbstractCppTypeHelper {
	private PackageTypeWrapper packageTypeWrapper; 
	
	public void fieldInitialize(FieldType field, ProtocolTypeWrapper protocol) {
		packageTypeWrapper=protocol.findPackageWrapper(field.getObjectRef(),field.getObjectRefId());
	}

//	public String getFieldDefinition(String name) {
//		return getTypeDeclaration() + " " + name;
//	}

	public String getTypeDeclaration()
	{
		return packageTypeWrapper.getClassName()+"*";
	}

	public String getInitValue() {
		return "NULL";
	}
	public String getInitCode(String name)
	{
		return name+"="+getInitValue()+";";
	}

	public String destroyCode(String name) {
		return "if("+name+") delete "+name+";\n"+name+"=NULL;\n";
	}

	public String readAssignCode(String name) {
		StringBuffer sb=new StringBuffer();
		sb.append(name+"=new "+getTypeName()+"();\n");
		sb.append("if (!"+name+"->deserializeContent(reader)) return false;");
		return sb.toString();
	}

	public String writeAssignCode(String name) {
		return writeCode(name);

	}
			
	public String writeCode(String name)
	{
		return name+"->serializeContent(writer);";
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

	public String getMapType() {
		return null;
	}

	public String getTypeName() {
		return packageTypeWrapper.getClassName();
	}

	public String isInCode(String value, String string) {
		// TODO Auto-generated method stub
		return null;
	}

	public boolean isMappable() {
		// TODO Auto-generated method stub
		return false;
	}
	
	public String getIncludeCode() {
		String dirName=packageTypeWrapper.getPacket_group_type().isMainGroup()?"packages":"packages_"+packageTypeWrapper.getPacket_group_type().getGroupName();
		return dirName+"/"+packageTypeWrapper.getClassName();
	}
	
	@Override
	public String equalsCode(String name) {
		// TODO Auto-generated method stub
		return super.equalsCode(name);
	}
	
	public String compareCode(String val1, String val2) {
		return "((!"+val1+" && ! "+val2 +")"+"||("+val1+" && "+val2+" && "+"(" + val1 + "->equals(" + val2 + "))))";
	}
}
