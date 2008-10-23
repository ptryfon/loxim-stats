package pl.edu.mimuw.loxim.protogen.lang.cpp;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.util.LinkedList;
import java.util.List;

import org.codehaus.plexus.PlexusContainerException;
import org.codehaus.plexus.component.repository.exception.ComponentLookupException;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.EnumType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.ItemType;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;
import pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper.CppTypeHelper;
import pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper.CppTypeHelperFactory;

public class CPPEnumsPartialGenerator extends CPPAbstractPackagesGenerator {

	public void generateEnumH(EnumTypeWrapper enum_type)
			throws GeneratorException {
		try {
			File f = new File(getEnum_target_directory(), enum_type
					.getFactoryClassName()
					+ ".h");
			if (!useOverwrittingFile(f)) {
				CodeCPPPrintStream out = new CodeCPPPrintStream(
						new FileOutputStream(f));
				out
						.printCode("#ifndef "
								+ enum_type.getFactoryClassName().toUpperCase()
								+ "_H_");
				out
						.printCode("#define "
								+ enum_type.getFactoryClassName().toUpperCase()
								+ "_H_");

				out.blankLine();
				out.printCode("#include <stdint.h>");
				out.blankLine();

				out.printCode("namespace " + NAMESPACE + "{");
				out.blankLine();

				out.printCode("enum " + enum_type.getClassName() + " {");
				Boolean first = true;
				out.indent();
				for (ItemType item : enum_type.getEnum_type().getItem()) {
					out.printCode((first ? "" : ",") + item.getName());
					first = false;
				}
				// out.printCode("ERROR");
				out.deindent();
				out.printCode("};");
				out.blankLine();

				out.printCode("struct " + enum_type.getMapClassName() + "{");
				{
					out.indent();
					for (ItemType item : enum_type.getEnum_type().getItem()) {
						out.printCode("unsigned " + item.getName() + ":1;");
					}
					out.deindent();
				}
				out.printCode("};");
				
//				out.printCode("extern "+enum_type.getMapClassName()+" "+(enum_type.getMapClassName().toUpperCase()+"_ALL;"));//+";";//.toUpperCase()+"="+EnumMapAll(enum_type.getEnum_type().getItem().size())+";");
//				out.printCode("extern "+enum_type.getMapClassName()+" "+(enum_type.getMapClassName().toUpperCase()+"_NONE;"));//+";";//.toUpperCase()+"="+EnumMapAll(enum_type.getEnum_type().getItem().size())+";");
//				out.printCode("extern "+enum_type.getMapClassName()+" "+(enum_type.getMapClassName().toUpperCase()+"_ODD;"));//.toUpperCase()+"="+EnumMapAll(enum_type.getEnum_type().getItem().size())+";");

				out.printCode("const "+enum_type.getMapClassName()+" "+(enum_type.getMapClassName().toUpperCase()+"_ALL"+"="+EnumMapAll(enum_type.getEnum_type().getItem().size())+";"));
				out.printCode("const "+enum_type.getMapClassName()+" "+(enum_type.getMapClassName().toUpperCase()+"_NONE"+"="+EnumMapNone(enum_type.getEnum_type().getItem().size())+";"));
				out.printCode("const "+enum_type.getMapClassName()+" "+(enum_type.getMapClassName().toUpperCase()+"_ODD"+"="+EnumMapOdd(enum_type.getEnum_type().getItem().size())+";"));
				
				
				out.blankLine();

				// class TestEnum{
				// enum months getEnumByValue(int value);
				// int getValueByEnum(enum months value);
				// };
				CppTypeHelper type_helper = CppTypeHelperFactory
						.getCppTypeHelper(enum_type.getEnum_type().getAsType());
				String asType = type_helper.getTypeDeclaration();
				out.printCode("class " + enum_type.getFactoryClassName() + "{");
				{
					out.printCode("public:");
					out.indent();
					out.printCode("static bool getEnumByValue(" + asType
							+ " value,enum " + enum_type.getClassName()
							+ "& res);");
					out.printCode("static bool getValueByEnum(enum "
							+ enum_type.getClassName() + " value," + asType
							+ "& res);");

					if (type_helper.isMappable()) {
						out.blankLine();
						out.printCode("static bool getEnumMapByValue("
								+ type_helper.getMapType() + " value,"
								+ enum_type.getMapClassName() + "& res);");
						out.printCode("static bool getValueByEnumMap("
								+ enum_type.getMapClassName() + " value,"
								+ type_helper.getMapType() + "& res);");
						out.printCode("static bool mapEquals("
								+ enum_type.getMapClassName() + " val1,"
								+ enum_type.getMapClassName() + " val2);");

					}
					out.deindent();
				}
				out.printCode("};//enum");
				out.printCode("};//namespace");
				out.printCode("#endif");
				out.blankLine();
				out.close();
			}
		} catch (PlexusContainerException e) {
			throw new GeneratorException(e);
		} catch (ComponentLookupException e) {
			throw new GeneratorException(e);
		} catch (FileNotFoundException e) {
			throw new GeneratorException(e);
		}
	}

	private String EnumMapNone(int size) {
		StringBuffer sb=new StringBuffer("{");
		for(int i=0; i<size-1; i++)
			sb.append("0,");
		sb.append("0}");
		return sb.toString();
	}

	private String EnumMapAll(int size) {
		StringBuffer sb=new StringBuffer("{");
		for(int i=0; i<size-1; i++)
			sb.append("1,");
		sb.append("1}");
		return sb.toString();
	}
	
	private String EnumMapOdd(int size) {
		StringBuffer sb=new StringBuffer("{");
		for(int i=0; i<size-1; i++)
			sb.append(((i+1)%2)+",");
		sb.append((size%2)+"}");
		return sb.toString();
	}


	public void generateEnumCpp(EnumTypeWrapper enum_type)
			throws GeneratorException {
		try {
			File f = new File(getEnum_target_directory(), enum_type
					.getFactoryClassName()
					+ ".cpp");
			if (!useOverwrittingFile(f)) {
				CodeCPPPrintStream out = new CodeCPPPrintStream(
						new FileOutputStream(f));
				out.printCode("#include \"" + enum_type.getFactoryClassName()
						+ ".h\"");
				out.blankLine();
				out.printCode("using namespace " + NAMESPACE + ";");
				out.blankLine();
	
				out.blankLine();
				
				CppTypeHelper type_helper = CppTypeHelperFactory
						.getCppTypeHelper(enum_type.getEnum_type().getAsType());
				String asType = type_helper.getTypeDeclaration();

				
				out.printCode("bool " + enum_type.getFactoryClassName()
						+ "::getEnumByValue(" + asType + " value,enum "
						+ enum_type.getClassName() + "& res){");

				{
					out.indent();
					for (ItemType item : enum_type.getEnum_type().getItem()) {
						out.printCode("if ("
								+ type_helper.compareCode("value", item
										.getValue()) + "){res= "
								+ item.getName() + ";return true;};");
					}
					out.printCode("return false;");
					out.deindent();
				}
				out.printCode("}");

				// int TestEnum::getValueByEnum(enum months value)
				// {
				// if(value==A1)
				// return 0;
				// }
				out.blankLine();
				out.printCode("bool " + enum_type.getFactoryClassName()
						+ "::getValueByEnum(enum " + enum_type.getClassName()
						+ " value," + asType + "& res){");
				{
					out.indent();
					out.printCode("switch (value){");
					{
						out.indent();
						for (ItemType item : enum_type.getEnum_type().getItem()) {
							out.printCode("case " + item.getName() + ": res= "
									+ item.getValue() + ";return true;");
						}
						out.printCode("default: return false;");
						out.deindent();
					}
					out.printCode("};");
					out.deindent();
				}
				out.printCode("}");

				if (type_helper.isMappable()) {
					out.printCode("bool " + enum_type.getFactoryClassName()
							+ "::getEnumMapByValue(" + type_helper.getMapType()
							+ " value," + enum_type.getMapClassName()
							+ "& res){");
					{
						out.indent();

						for (ItemType item : enum_type.getEnum_type().getItem()) {
							out.printCode("res."
									+ item.getName()
									+ "=("
									+ type_helper.isInCode(item.getValue(),
											"value") + "?1:0);");
						}
						out.printCode("return true;");
						out.deindent();
					}
					out.printCode("}");

					out.blankLine();
					out.printCode("bool " + enum_type.getFactoryClassName()
							+ "::getValueByEnumMap("
							+ enum_type.getMapClassName() + " value,"
							+ type_helper.getMapType() + "& res){");
					{
						out.indent();
						out.printCode("res=0;");
						for (ItemType item : enum_type.getEnum_type().getItem()) {
							out.printCode("if (value." + item.getName()
									+ ") res|=" + item.getValue() + ";");
							// ("+type_helper.isInCode(item.getValue(),"value")+"?1:0)");
						}
						out.printCode("return true;");
						out.deindent();
					}
					out.printCode("};");

					out.printCode("bool " + enum_type.getFactoryClassName()+"::"
							+ "mapEquals(" + enum_type.getMapClassName()
							+ " val1," + enum_type.getMapClassName()
							+ " val2){");
					{
						out.indent();
						StringBuffer sb = new StringBuffer("return ");
						for (ItemType item : enum_type.getEnum_type().getItem()) {
							sb.append("(val1." + item.getName() + "==val2."
									+ item.getName() + ")&&");
							// out.printCode("if (val1."+item.getName()+")
							// res|="+item.getValue()+";");
							// ("+type_helper.isInCode(item.getValue(),"value")+"?1:0)");
						}
						sb.append("(true);");
						out.printCode(sb);
						out.deindent();
					}
					out.printCode("};");
					out.blankLine();
					out.close();
				}
			}
		} catch (PlexusContainerException e) {
			throw new GeneratorException(e);
		} catch (ComponentLookupException e) {
			throw new GeneratorException(e);
		} catch (FileNotFoundException e) {
			throw new GeneratorException(e);
		}
	}

	public void generateEnumsH(ProtocolTypeWrapper wrapper)
			throws GeneratorException {
		try {
			File f = new File(getEnum_target_directory(), "enums.h");
			if (!useOverwrittingFile(f)) {
				CodeCPPPrintStream out = new CodeCPPPrintStream(
						new FileOutputStream(f));
				out.printCode("#ifndef __ENUMS_H");
				out.printCode("#define __ENUMS_H");
				out.blankLine();
				for (EnumType e : wrapper.getProtocol().getEnums().getEnum()) {
					EnumTypeWrapper etw = new EnumTypeWrapper(e, wrapper);
					out.printCode("#include \"" + etw.getFactoryClassName()
							+ ".h\"");
				}
				out.blankLine();
				out.printCode("#endif");
				out.blankLine();
				out.close();
			}
		} catch (FileNotFoundException e) {
			throw new GeneratorException(e);
		}
	}

	public void generateEnumsMakefile(ProtocolTypeWrapper wrapper)
			throws GeneratorException {
		File f = new File(getEnum_target_directory(), "Makefile");
		if (!useOverwrittingFile(f)) {
			List<String> names = new LinkedList<String>();
			for (EnumType e : wrapper.getProtocol().getEnums().getEnum()) {
				EnumTypeWrapper etw = new EnumTypeWrapper(e, wrapper);
				names.add(etw.getFactoryClassName());
			}

			MakefileGenerator mg = new MakefileGenerator();
			mg.generate(f, 2, "enums", names, new LinkedList<String>());
		}
	}
	
	
}
