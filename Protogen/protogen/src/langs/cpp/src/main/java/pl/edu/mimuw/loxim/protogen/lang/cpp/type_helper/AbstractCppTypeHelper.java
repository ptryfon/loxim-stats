package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import org.codehaus.plexus.PlexusContainerException;
import org.codehaus.plexus.component.repository.exception.ComponentLookupException;

import pl.edu.mimuw.loxim.protogen.api.CodeHelper;
import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;

public abstract class AbstractCppTypeHelper implements CppTypeHelper {

	public String getGetter(String name) {
		return getTypeDeclaration() + " get" + CodeHelper.upperFirstChar(name)
				+ "(){return " + name + ";};";
	}

	public String getSetter(String name) {
		return "void set" + CodeHelper.upperFirstChar(name) + "("
				+ getTypeDeclaration() + " a_" + name + "){" + name + "=a_" + name
				+ ";};";
	}

	public String getFieldDefinition(String name) {
		return getTypeDeclaration() + " " + name;
	}

	public String getDefineConstructor(String value) {
		return value;
	}

	abstract public String getTypeDeclaration();

	public String getInitValue() {
		return "0";
	}
	public String getInitCode(String name)
	{
		return name+"="+getInitValue()+";";
	}

	public String destroyCode(String name) {
		return "";
	}

	public String readAssignCode(String name) {
		return assign(name,readCode());
	}

	private String assign(String name, String value) {
		return name+"="+value+";";
	}

	
	public String readCode()
	{
		return "reader->read"
		+ CodeHelper.upperFirstChar(getTypeName()) + "()";
	}
	
	public String writeCode(String value)
	{
		return "writer->write" + CodeHelper.upperFirstChar(getTypeName()) + "("
		+ value + ");";
	}

	public String equalsCode(String name) {
		return "if (!"
				+ compareCode("get" + CodeHelper.upperFirstChar(name) + "()",
						"p->get" + CodeHelper.upperFirstChar(name) + "()")
				+ ") return false;";
	}

	public String compareCode(String val1, String val2) {
		return "(" + val1 + "==" + val2 + ")";
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
}
