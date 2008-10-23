package pl.edu.mimuw.loxim.protogen.lang.cpp;

import java.io.File;
import java.util.LinkedList;
import java.util.List;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.PacketGroupType;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class CPPGlobalsPartialGenerator extends CPPAbstractPackagesGenerator{
	
	public void generateGlobalMakefile(ProtocolTypeWrapper pr)
			throws GeneratorException {
		File f = new File(new File(getTarget_directory(), "protocol"),
				"Makefile");
		List<String> names = new LinkedList<String>();

		for (PacketGroupType p : pr.getProtocol().getPacketGroups()
				.getPacketGroup()) {
			PackageGroupTypeWrapper pa = new PackageGroupTypeWrapper(p, pr);
			if (pa.isMainGroup()) {
				names.add("packages");
			} else {
				names.add("packages_" + pa.getGroupName());
			}
		}
		names.add("auth");
		names.add("layers");
		names.add("base_packages");
		names.add("enums");
		names.add("pstreams");
		names.add("ptools");
		names.add("sockets");
		names.add("streams");
		names.add("tests");
		MakefileGenerator mg = new MakefileGenerator();
		mg.generate(f, 1, "protogen", new LinkedList<String>(), names);
	}
}
