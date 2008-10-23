package pl.edu.mimuw.loxim.protogen.lang.cpp;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.codehaus.plexus.PlexusContainerException;
import org.codehaus.plexus.component.repository.exception.ComponentLookupException;

import pl.edu.mimuw.loxim.protogen.api.CodeHelper;
import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.StreamsHelpers;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.PacketType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.Type;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageTypeWrapper;
import pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper.CppTypeHelper;
import pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper.CppTypeHelperFactory;

public class CPPPackagesGenerator extends CPPAbstractPackagesGenerator {

	// ------------------------------------------------------
	private void generateForPackage(PackageTypeWrapper _package) throws GeneratorException {
		generateHeaderFileForPackage(_package);
		generateCppFileForPackage(_package);
	}

	private void generateHeaderFileForPackage(PackageTypeWrapper _package) throws GeneratorException {
		try {
			File f = new File(getPackage_target_directory(_package.getPacket_group_type()), _package.getClassName() + ".h");
			if (!useOverwrittingFile(f)) {
				CodeCPPPrintStream out = new CodeCPPPrintStream(new FileOutputStream(f));

				out.printCode("#ifndef " + _package.getClassName().toUpperCase() + "_H_");
				out.printCode("#define " + _package.getClassName().toUpperCase() + "_H_");

				out.blankLine();
				out.printCode("#include <stdint.h>");
				out.printCode("#include \"../ptools/Package.h\"");
				out.printCode("#include \"../ptools/CharArray.h\"");
				out.printCode("#include \"../enums/enums.h\"");
				if (!_package.getExtendClassName().equals("Package")) {
					out.printCode("#include \"" + _package.getExtendClassName() + ".h\"");
				}
				for (FieldType field : _package.getPackage().getField()) {
					String include_code_core = CppTypeHelperFactory.getCppTypeHelper(field, _package.getProtocol()).getIncludeCode();
					if (include_code_core != null) {
						out.printCode("#include \"../" + include_code_core + ".h\"");
					}
				}

				out.blankLine();
				out.printCode("namespace " + NAMESPACE + "{");

				if (!_package.isAbstract()) {
					out.blankLine();
					out.printCode("#define ID_" + _package.getClassName() + " " + _package.getPackage().getIdValue());
				}

				out.blankLine();
				out.printCode("class "
				// + (_package.isAbstract() ? " abstract " : "")
						+ _package.getClassName() + " : public " + (_package.getExtendClassName() != null ? _package.getExtendClassName() : "Package"));
				out.printCode("{");
				{
					out.indent();
					out.printCode("private:");
					{
						out.indent();
						for (FieldType field : _package.getPackage().getField()) {
							out.printCode(CppTypeHelperFactory.getCppTypeHelper(field, _package.getProtocol()).getFieldDefinition(field.getName()) + ";");
							if (field.isNullable() && field.getType() == Type.VARUINT) {
								out.printCode("bool bnull_" + field.getName() + ";");
							}
						}

						out.deindent();
					}
					out.printCode("public:");
					{
						out.indent();
						out.printCode(_package.getClassName() + "();");
						out.printCode("~" + _package.getClassName() + "();");

						if (_package.getPackage().getField().size() > 0) {
							out.printCode(_package.getClassName() + "(");
							{
								out.indent();
								constructorParams(out, _package, true, true);
								out.deindent();
							}
						}

						out.blankLine();
						out.printCode("virtual bool equals(Package* _package);");
						out.blankLine();

						for (FieldType field : _package.getPackage().getField()) {
							CppTypeHelper typeHelper = CppTypeHelperFactory.getCppTypeHelper(field, _package.getProtocol());
							out.printCode(typeHelper.getGetter(field.getName()));
							out.printCode(typeHelper.getSetter(field.getName()));
							out.blankLine();

						}

						// if (!_package.isAbstract()) {
						out.blankLine();
						String id_type = _package.getProtocol().getMainGroup().getPacket_group_type().getIdType().value();
						out.printCode(CppTypeHelperFactory.getCppTypeHelper(id_type).getTypeDeclaration() + " getPackageType()"
								+ (_package.isAbstract() ? "=0;" : ("{return ID_" + _package.getClassName() + ";};")));
						// }

						out.deindent();
					}
					out.printCode("protected:");
					{
						out.indent();
						out.printCode("void serializeW(PackageBufferWriter *writter);");
						out.printCode("bool deserializeR(PackageBufferReader *reader);");
						out.deindent();
					}
					out.deindent();
				}
				out.printCode("};//class");
				out.blankLine();
				out.printCode("}//namespace");

				out.printCode("#endif /*define " + _package.getClassName().toUpperCase() + "_H_" + "*/");
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

	private void generateCppFileForPackage(PackageTypeWrapper _package) throws GeneratorException {
		try {
			File f = new File(getPackage_target_directory(_package.getPacket_group_type()), _package.getClassName() + ".cpp");
			System.out.println("  Working on file: " + f.getAbsolutePath());
			if (!useOverwrittingFile(f)) {

				CodeCPPPrintStream out = new CodeCPPPrintStream(new FileOutputStream(f));
				out.printCode("#include <string.h>");
				out.printCode("#include <stdlib.h>");
				out.printCode("#include \"" + _package.getClassName() + ".h\"");
				out.printCode("#include \"ptools/PackageBufferWriter.h\"");
				out.printCode("#include \"ptools/PackageBufferReader.h\"");

				out.blankLine();
				out.printCode("using namespace " + NAMESPACE + ";");

				if (_package.getPackage().getField().size() > 0) {
					// Constructor
					out.blankLine();
					out.printCode(_package.getClassName() + "::" + _package.getClassName() + "(\\");
					{
						out.indent();
						out.indent();
						constructorParams(out, _package, false, false);
						out.deindent();
						out.deindent();
					}
					out.printCode("{");
					{
						out.indent();
						for (FieldType field : _package.getPackage().getField()) {
							out.printCode(field.getName() + "=a_" + field.getName() + ";");
							if (field.getType() == Type.VARUINT && field.isNullable()) {
								out.printCode("bnull_" + field.getName() + "=a_bnull_" + field.getName() + ";");
							}
						}
						out.deindent();
					}
					out.printCode("}");
				}

				// Non-parameter constructor
				out.blankLine();
				out.printCode(_package.getClassName() + "::" + _package.getClassName() + "()");
				out.printCode("{");
				{
					out.indent();
					for (FieldType field : _package.getPackage().getField()) {

						CppTypeHelper typeHelper = CppTypeHelperFactory.getCppTypeHelper(field, _package.getProtocol());
						out.printCode(typeHelper.getInitCode(field.getName()));
					}
					out.deindent();
				}
				out.printCode("}");

				// Destructor
				out.blankLine();
				out.printCode(_package.getClassName() + "::~" + _package.getClassName() + "()");
				out.printCode("{");
				{
					out.indent();
					for (FieldType field : _package.getPackage().getField()) {
						CppTypeHelper typeHelper = CppTypeHelperFactory.getCppTypeHelper(field, _package.getProtocol());
						out.printCode(typeHelper.destroyCode(field.getName()));

					}
					out.deindent();
				}
				out.printCode("}");
				// Serializer
				out.blankLine();
				out.printCode("void " + _package.getClassName() + "::serializeW(PackageBufferWriter *writer)");
				out.printCode("{");
				{
					out.indent();
					out.printCode(_package.getExtendClassName() + "::serializeW(writer);");
					for (FieldType field : _package.getPackage().getField()) {
						CppTypeHelper typeHelper = CppTypeHelperFactory.getCppTypeHelper(field, _package.getProtocol());
						out.printCode(typeHelper.writeCode(field.getName()));
					}
					out.deindent();
				}
				out.printCode("}");
				// Deserializer
				out.blankLine();
				out.printCode("bool " + _package.getClassName() + "::deserializeR(PackageBufferReader *reader)");
				out.printCode("{");
				{
					out.indent();
					out.printCode("if (!" + _package.getExtendClassName() + "::deserializeR(reader)) return false;");
					for (FieldType field : _package.getPackage().getField()) {

						CppTypeHelper typeHelper = CppTypeHelperFactory.getCppTypeHelper(field, _package.getProtocol());
						out.printCode(typeHelper.readAssignCode(field.getName()));

					}
					out.printCode("return true;");
					out.deindent();
				}
				out.printCode("}");
				// Equals
				out.blankLine();
				out.printCode("bool " + _package.getClassName() + "::equals(Package *_package)");
				out.printCode("{");
				{
					out.indent();
					out.printCode("if (!_package) return false;");
					/*
					 * TODO: Ta linia jest buggy dla nie numerycznych typów identyfikuj±cych pakiety
					 */
					out.printCode("if (_package->getPackageType()!=this->getPackageType()) return false;");
					out.printCode("if (!" + _package.getExtendClassName() + "::equals(_package)) return false;");
					out.printCode(_package.getClassName() + "* p=(" + _package.getClassName() + "*)_package;");
					for (FieldType field : _package.getPackage().getField()) {

						CppTypeHelper typeHelper = CppTypeHelperFactory.getCppTypeHelper(field, _package.getProtocol());
						out.printCode(typeHelper.equalsCode(field.getName()));

					}
					out.printCode("return true;");
					out.deindent();
				}
				out.printCode("}");
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

	private void constructorParams(CodeCPPPrintStream out, PackageTypeWrapper _package, boolean semicolon, boolean declaration)
			throws PlexusContainerException, ComponentLookupException, GeneratorException {
		int size = _package.getPackage().getField().size();
		int i = 0;
		for (FieldType field : _package.getPackage().getField()) {
			String base = CppTypeHelperFactory.getCppTypeHelper(field, _package.getProtocol()).getFieldDefinition("a_" + field.getName());
			if (i + 1 < size) {
				out.printCode(base + ",\\");
			} else {
				out.printCode(base);
			}
			i++;
		}
		for (FieldType field : _package.getPackage().getField()) {
			if (field.getType() == Type.VARUINT && field.isNullable()) {
				String base = ",bool a_bnull_" + field.getName();
				if (declaration)
					base += "=false";
				out.printCode(base);
			}
		}
		out.printCode(")" + (semicolon ? ";" : ""));
	}

	private void generatePackagesMakefile(PackageGroupTypeWrapper group) throws GeneratorException {
		File f = new File(getPackage_target_directory(group), "Makefile");
		if (!useOverwrittingFile(f)) {
			List<String> names = new LinkedList<String>();
			for (PacketType e : group.getPacket_group_type().getPackets().getPacket()) {
				PackageTypeWrapper etw = new PackageTypeWrapper(e, group);
				names.add(etw.getClassName());
			}

			names.add(CodeHelper.upperFirstChar(group.getGroupName() + "PackagesFactory"));
			MakefileGenerator mg = new MakefileGenerator();
			mg.generate(f, 2, group.isMainGroup() ? "packages" : "packages_" + group.getGroupName(), names, new LinkedList<String>());
		}
	}

	private void generatePackagesFactoryH(PackageGroupTypeWrapper group) throws GeneratorException {
		try {
			File f = new File(getPackage_target_directory(group), group.getFactoryClassName() + ".h");
			if (!useOverwrittingFile(f)) {
				CodeCPPPrintStream out = new CodeCPPPrintStream(f);
				String cap = group.getGroupName().toUpperCase();
				out.printCode("#ifndef " + cap + "PACKAGESFACTORY_H_");
				out.printCode("#define " + cap + "PACKAGESFACTORY_H_");
				out.blankLine();
				out.printCode("#include <stdint.h>");
				out.printCode("#include \"../ptools/Package.h\"");
				out.blankLine();
				out.printCode("namespace protocol{");
				out.blankLine();
				out.printCode("class " + group.getFactoryClassName() + "{");
				out.printCode("public:");
				out.indent();
				out.printCode("static Package *createPackageNotInit(uint8_t type);");
				out.printCode("static Package *createPackage(uint8_t type, char* data);");
				out.printCode("static Package *createPackageContent(uint8_t type,PackageBufferReader* reader);");
				out.deindent();

				out.printCode("};}");
				out.printCode("#endif /*" + cap + "PACKAGESFACTORY_H_*/");
				out.close();
			}
		} catch (IOException e) {
			throw new GeneratorException(e);
		}
	};

	private void generatePackagesFactoryCpp(PackageGroupTypeWrapper group) throws GeneratorException {
		try {
			File f = new File(getPackage_target_directory(group), group.getFactoryClassName() + ".cpp");
			if (!useOverwrittingFile(f)) {
				Map<String, String> translations = new HashMap<String, String>();
				translations.put("${PackagesFactory}", group.getFactoryClassName());

				CodeCPPPrintStream out = new CodeCPPPrintStream(f);
				StreamsHelpers.writeWholeInputStream(out, this.getClass().getResourceAsStream("PackagesFactory.cpp-prefix"), translations);

				if (group.isMainGroup()) {
					out.blankLine();
					out.printCode("#include \"../base_packages/ASCPingPackage.h\"");
					out.printCode("#include \"../base_packages/ASCPongPackage.h\"");
					out.blankLine();
				}

				for (PacketType e : group.getPacket_group_type().getPackets().getPacket()) {
					PackageTypeWrapper etw = new PackageTypeWrapper(e, group);
					out.printCode("#include \"" + etw.getClassName() + ".h\"");
				}
				StreamsHelpers.writeWholeInputStream(out, this.getClass().getResourceAsStream("PackagesFactory.cpp-infix"), translations);
				out.indent();
				out.indent();
				out.blankLine();

				if (group.isMainGroup()) {
					out.blankLine();
					out.printCode("case ID_ASCPingPackage: return new ASCPingPackage();");
					out.printCode("case ID_ASCPongPackage: return new ASCPongPackage();");
					out.blankLine();
				}

				for (PacketType e : group.getPacket_group_type().getPackets().getPacket()) {
					PackageTypeWrapper etw = new PackageTypeWrapper(e, group);
					if (!etw.isAbstract()) {
						out.printCode("case ID_" + etw.getClassName() + ": return new " + etw.getClassName() + "();");
					}
				}
				out.deindent();
				out.deindent();
				out.blankLine();
				StreamsHelpers.writeWholeInputStream(out, this.getClass().getResourceAsStream("PackagesFactory.cpp-suffix"), translations);
				out.close();
			}
		} catch (IOException e) {
			throw new GeneratorException(e);
		}
	}

	public void generateForPackageGroup(PackageGroupTypeWrapper package_group) throws GeneratorException {
		System.out.println(" Working on package group: " + package_group.getGroupName());
		for (PacketType p : package_group.getPacket_group_type().getPackets().getPacket()) {
			generateForPackage(new PackageTypeWrapper(p, package_group));
		}
		generatePackagesFactoryCpp(package_group);
		generatePackagesFactoryH(package_group);
		generatePackagesMakefile(package_group);
	};

}
