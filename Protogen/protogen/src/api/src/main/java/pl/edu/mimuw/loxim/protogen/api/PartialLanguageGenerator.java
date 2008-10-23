package pl.edu.mimuw.loxim.protogen.api;

import java.io.File;

import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public interface PartialLanguageGenerator {
	/* Plexus role identifier */
	static String ROLE = PartialLanguageGenerator.class.getName();

	/**
	 * call after "setDescriptor" and after "setOverwriteSrcDirectory" but
	 * before any generate method"
	 */

	public void init() throws GeneratorException;

	public ProtocolTypeWrapper getDescriptor();

	public void setDescriptor(ProtocolTypeWrapper descriptor);

	public void setTarget_directory(File directory);

	public File getTarget_directory();

	public File getOverwrite_src_directory();

	public void setOverwrite_src_directory(File overwrite_src_directory);

	public void generatePackageGroup(PackageGroupTypeWrapper package_group)
			throws GeneratorException;

	public void generateEnum(EnumTypeWrapper enum_type)
			throws GeneratorException;

	public void generateGlobals() throws GeneratorException;

	public void generateTests() throws GeneratorException;
}
