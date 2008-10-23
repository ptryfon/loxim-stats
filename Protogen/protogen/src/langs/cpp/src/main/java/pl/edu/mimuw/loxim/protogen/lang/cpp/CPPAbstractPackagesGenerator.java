package pl.edu.mimuw.loxim.protogen.lang.cpp;

import java.io.File;

import pl.edu.mimuw.loxim.protogen.api.AbstractPackageGenerator;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;

public abstract class CPPAbstractPackagesGenerator extends AbstractPackageGenerator{

	public static final String NAMESPACE = "protocol";
	private File target_directory;

	
	// ---------------- Directories -------------------------
	public File getTarget_directory() {
		return target_directory;
	}

	public void setTarget_directory(File directory) {
		target_directory = directory;
	}

	public File getEnum_target_directory() {
		File f = new File(new File(getTarget_directory(), "protocol"), "enums");
		f.mkdirs();
		return f;
	}

	public File getPackage_target_directory(PackageGroupTypeWrapper group) {
		String name;
		if ((group.getGroupName() == null) || (group.getGroupName().equals(""))) {
			name = "packages";
		} else {
			name = "packages_" + group.getGroupName();
		}
		File f = new File(new File(getTarget_directory(), "protocol"), name);
		f.mkdirs();
		return f;
	}


}
