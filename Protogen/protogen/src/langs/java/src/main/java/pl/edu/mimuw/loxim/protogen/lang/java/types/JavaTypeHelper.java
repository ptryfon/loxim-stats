package pl.edu.mimuw.loxim.protogen.lang.java.types;

import java.math.BigInteger;
import java.util.Arrays;

import pl.edu.mimuw.loxim.protogen.api.CodeHelper;
import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.Type;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class JavaTypeHelper {
	public ProtocolTypeWrapper protocol_wrapper;

	public JavaTypeHelper(ProtocolTypeWrapper protocol) {
		protocol_wrapper = protocol;
	}

	public void setProtocol_wrapper(ProtocolTypeWrapper protocol_wrapper) {
		this.protocol_wrapper = protocol_wrapper;
	}

	public String getTypeName(Type t) {
		return getTypeName(t, null, null);
	};

	public String getTypeName(Type t, String ref, String id) {
		switch (t) {
		case BOOL:
			return "boolean";
		case DOUBLE:
			return "double";
		case SINT_8:
			return "byte";
		case SINT_16:
			return "short";
		case SINT_32:
			return "int";
		case SINT_64:
			return "long";
		case UINT_8:
			return "short";
		case UINT_16:
			return "int";
		case UINT_32:
			return "long";
		case UINT_64:
			return BigInteger.class.getName();
		case VARUINT:
			return "Long";
		case SSTRING:
		case STRING:
			return "String";
		case ENUM:
			return CodeHelper.upperFirstChar(ref + "Enum");
		case ENUM_MAP:
			return "EnumSet<" + CodeHelper.upperFirstChar(ref + "Enum") + ">";
		case PACKAGE:
			return protocol_wrapper.findPackageWrapper(ref, id).getClassName();
		case PACKAGE_MAP:
			return protocol_wrapper.findGroupByName(ref).getSuperclassName();
		case BYTES:
			return "byte[]";
		default:
			return "Not supported";
		}
	}
	
	public String getDefaultValue(Type t, String ref, String id) {
		switch (t) {
		case BOOL:
			return "true";
		case DOUBLE:
			return "0.0";
		case SINT_8:
			return "0";
		case SINT_16:
			return "0";
		case SINT_32:
			return "0";
		case SINT_64:
			return "0";
		case UINT_8:
			return "0";
		case UINT_16:
			return "0";
		case UINT_32:
			return "0";
		case UINT_64:
			return BigInteger.class.getName()+".ZERO";
		case VARUINT:
			return "new Long(0)";
		case SSTRING:
		case STRING:
			return "null";
		case ENUM:
			return "null";
		case ENUM_MAP:
			return "EnumSet.noneOf(" + CodeHelper.upperFirstChar(ref + "Enum.class") + ")";
		case PACKAGE:
			return "null";
		case PACKAGE_MAP:
			return "null";
		case BYTES:
			return "new byte[0]";
		default:
			return "Not supported";
		}
	}
	

	public String getTypeValue(Type t, String value) throws GeneratorException {
		return getTypeValue(t, null, null, value);
	}

	public String getTypeValue(Type t, String ref, String id, String value)
			throws GeneratorException {
		switch (t) {
		case BOOL:
			if ((value.equalsIgnoreCase("true"))
					|| (value.equalsIgnoreCase("false"))) {
				return value.toLowerCase();
			} else {
				throw new GeneratorException("Unknown bool value: " + value);
			}
		case DOUBLE:
		case SINT_8:
		case SINT_16:
		case SINT_32:
		case SINT_64:
		case UINT_8:
		case UINT_16:
		case UINT_32:
		case VARUINT:
			return "(" + getTypeName(t) + ")" + value;
		case UINT_64:
			return "(pl.edu.mimuw.loxim.protogen.lang.java.template.util.TypeUtil.str2bigInteger(\"" + value + "\"))";
		case SSTRING:
		case STRING:
			return "\"" + value + "\"";
		case BYTES:
			return "\"" + value + "\".getBytes(\"US-ASCII\")";			
		case ENUM:
		case ENUM_MAP:
		case PACKAGE:
		case PACKAGE_MAP:
		default:
			return "Not supported";
		}
	}

	public String getTypeGetter(Type t, String ref, String id, String name)
			throws GeneratorException {
		return "public " + getTypeName(t, ref, id) + " get"
				+ CodeHelper.upperFirstChar(name) + "(){return " + name + ";};";
	}

	public String getTypeSetter(Type t, String ref, String id, String name)
			throws GeneratorException {
		return "public void set" + CodeHelper.upperFirstChar(name) + "("
				+ getTypeName(t, ref, id) + " a_" + name + "){ "+name + "=a_"
				+ name + ";};";

	}

	public String getEqualsCode(Type t, String ref, String id, String v1,
			String v2) {
		switch (t) {
		case BOOL:
		case DOUBLE:
		case SINT_8:
		case SINT_16:
		case SINT_32:
		case SINT_64:
		case UINT_8:
		case UINT_16:
		case UINT_32:
			return "(" + v1 + "==" + v2 + ")";
		case BYTES:
			return "java.util.Arrays.equals("+v1+","+v2+")";
		case VARUINT:
		case SSTRING:
		case STRING:
		case UINT_64:
		case ENUM:
		case ENUM_MAP:
		case PACKAGE:
		case PACKAGE_MAP:
		default:
			return "((" + v1 + "==" + v2 + ")||((" + v1 + "!=null)&&(" + v2
			+ "!=null)&&((" + v1 + ").equals(" + v2 + "))))";
		}
	}

	public String getWriteCode(Type t, String ref, String id, String name,
			String writer) {
		switch (t) {
		case VARUINT:
		case BOOL:
		case DOUBLE:
		case SINT_8:
		case SINT_16:
		case SINT_32:
		case SINT_64:
		case UINT_8:
		case UINT_16:
		case UINT_32:
		case SSTRING:
		case UINT_64:
		case BYTES:
		case STRING:
			return writer
					+ ".write"
					+ name2writeReadNameSuffix(t) + "(" + name + ");";
		case ENUM:
		{
			EnumTypeWrapper etw=protocol_wrapper.findEnumWrapper(ref);
			return getWriteCode(etw.getEnum_type().getAsType(), null, null, name+".getValue()", writer);
		}
		case ENUM_MAP:
		{
			EnumTypeWrapper etw=protocol_wrapper.findEnumWrapper(ref);
			return getWriteCode(etw.getEnum_type().getAsType(), null, null, etw.getClassName()+".toEnumMapValue("+name+")", writer);
		}
		case PACKAGE:
		{
//			PackageTypeWrapper ptw=protocol_wrapper.findPackageWrapper(ref, id);
			StringBuffer sb=new StringBuffer();
			sb.append(name+".serializeContent("+writer+");\n");
			return sb.toString();
		}
		case PACKAGE_MAP:
		{
//			PackageTypeWrapper ptw=protocol_wrapper.findPackageWrapper(ref, id);
			StringBuffer sb=new StringBuffer();
			sb.append(name+".serializeContent("+writer+");");
			return sb.toString();
		}
		default:
			return "UNSUPPORTED";
		}
	}

	private String name2writeReadNameSuffix(Type t) {
		 return CodeHelper.upperFirstChar(t.toString().replace("SINT_","INT").replace("_", "")
					.toLowerCase()).replace("Sstring", "SString");
	}

	public String getReadCode(Type t, String ref, String id, String name,
			String reader) {
		switch (t) {
		case VARUINT:
		case BOOL:
		case DOUBLE:
		case SINT_8:
		case SINT_16:
		case SINT_32:
		case SINT_64:
		case UINT_8:
		case UINT_16:
		case UINT_32:
		case SSTRING:
		case BYTES:
		case STRING:
		case UINT_64:
			return name
					+ "="
					+ reader
					+ ".read"
					+ name2writeReadNameSuffix(t) + "();";					
		case ENUM:
		{
			EnumTypeWrapper etw=protocol_wrapper.findEnumWrapper(ref);
			StringBuffer sb=new StringBuffer(getTypeName(etw.getEnum_type().getAsType())+" "+getReadCode(etw.getEnum_type().getAsType(), null, null, "tmp_"+name, reader)+"\n");
			sb.append(name+"="+etw.getClassName()+".createEnumByValue(tmp_"+name+");");
			return sb.toString();
		}
		case ENUM_MAP:
		{
			EnumTypeWrapper etw=protocol_wrapper.findEnumWrapper(ref);
			StringBuffer sb=new StringBuffer(getTypeName(etw.getEnum_type().getAsType())+" "+getReadCode(etw.getEnum_type().getAsType(), null, null, "tmp_"+name, reader)+"\n");
			sb.append(name+"="+etw.getClassName()+".createEnumMapByValue(tmp_"+name+");");
			return sb.toString();
		}
		case PACKAGE:
		{
			PackageTypeWrapper ptw=protocol_wrapper.findPackageWrapper(ref, id);
			StringBuffer sb=new StringBuffer();
			sb.append(name+"=new "+ptw.getClassName()+"();\n");
			sb.append(name+".deserializeContent("+reader+");");
			return sb.toString();
		}
		case PACKAGE_MAP:
		{
			PackageGroupTypeWrapper pgtw=protocol_wrapper.findGroupByName(ref);
			StringBuffer sb=new StringBuffer();
			sb.append(name+"="+pgtw.getProtocol().getLangMetadataMap("java").get("packageName")+"."+getJavaPackageNameSuffixForGroup(pgtw)+"."+pgtw.getFactoryClassName()+".getInstance().createPackage("+id+");\n");
			sb.append(name+".deserializeContent("+reader+");");
			return sb.toString();
		}
		default:
			return "UNSUPPORTED";
		}
	}
	
	public static String getJavaPackageNameSuffixForGroup(
			PackageGroupTypeWrapper pgtw) {
		return (		
				(pgtw.isMainGroup()) ? "packages"
				: "packages_" + pgtw.getGroupName().toLowerCase());
	}

}