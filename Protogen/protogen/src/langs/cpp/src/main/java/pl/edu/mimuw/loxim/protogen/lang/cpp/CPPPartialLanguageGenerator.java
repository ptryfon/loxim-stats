package pl.edu.mimuw.loxim.protogen.lang.cpp;

import java.io.File;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.PartialLanguageGenerator;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class CPPPartialLanguageGenerator implements PartialLanguageGenerator {

	private final CPPPackagesGenerator cppPackagesGenerator = new CPPPackagesGenerator();
	private final CPPGlobalsPartialGenerator cppGlobalsPartialGenerator = new CPPGlobalsPartialGenerator();
	private final CPPEnumsPartialGenerator cppEnumsPartialGenerator = new CPPEnumsPartialGenerator();
	private final CPPTestsPartialGenerator cppTestsPartialGenerator = new CPPTestsPartialGenerator();

	private ProtocolTypeWrapper protocol_wrapper;

	public ProtocolTypeWrapper getDescriptor() {
		return protocol_wrapper;
	}

	public void setDescriptor(ProtocolTypeWrapper descriptor) {
		this.protocol_wrapper = descriptor;
	}

	public File getOverwrite_src_directory() {
		return cppPackagesGenerator.getOverwrite_src_directory();
	}

	public File getTarget_directory() {
		return cppPackagesGenerator.getTarget_directory();
	}

	public void setOverwrite_src_directory(File overwrite_src_directory) {
		cppPackagesGenerator
				.setOverwrite_src_directory(overwrite_src_directory);
		cppGlobalsPartialGenerator
				.setOverwrite_src_directory(overwrite_src_directory);
		cppEnumsPartialGenerator
				.setOverwrite_src_directory(overwrite_src_directory);
		cppTestsPartialGenerator.setOverwrite_src_directory(overwrite_src_directory);
	}

	public void setTarget_directory(File directory) {
		cppPackagesGenerator.setTarget_directory(directory);
		cppGlobalsPartialGenerator.setTarget_directory(directory);
		cppEnumsPartialGenerator.setTarget_directory(directory);
		cppTestsPartialGenerator.setTarget_directory(directory);
	}

	public void generateEnum(EnumTypeWrapper enum_type)
			throws GeneratorException {
		cppEnumsPartialGenerator.generateEnumH(enum_type);
		cppEnumsPartialGenerator.generateEnumCpp(enum_type);
	}

	public void generateGlobals() throws GeneratorException {
		cppEnumsPartialGenerator.generateEnumsH(protocol_wrapper);
		cppEnumsPartialGenerator.generateEnumsMakefile(protocol_wrapper);
		cppGlobalsPartialGenerator.generateGlobalMakefile(protocol_wrapper);
	}

	public void generatePackageGroup(PackageGroupTypeWrapper package_group)
			throws GeneratorException {
		cppPackagesGenerator.generateForPackageGroup(package_group);

	}

	
	public void generateTests() throws GeneratorException {
		cppTestsPartialGenerator.generateTests(protocol_wrapper);
	}

	public void init() throws GeneratorException {
		
	}

}
