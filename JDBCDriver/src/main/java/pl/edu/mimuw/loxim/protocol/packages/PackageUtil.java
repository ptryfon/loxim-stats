package pl.edu.mimuw.loxim.protocol.packages;

import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.BadPackageException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageIO;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public final class PackageUtil {

	public static String toString(A_sc_errorPackage errorPac) {
		String msg = errorPac.getDescription();
		StringBuilder buf = new StringBuilder(msg.length() + 20);
		buf.append("ERROR #");
		buf.append(errorPac.getError_code().getValue());
		buf.append(" at [");
		buf.append(errorPac.getLine());
		buf.append(", ");
		buf.append(errorPac.getCol());
		buf.append("]");
		if (errorPac.getError_object_id() != 0) {
			buf.append(" on object #");
			buf.append(errorPac.getError_object_id());
		}
		buf.append(": ");
		buf.append(errorPac.getDescription());
		return buf.toString();
	}
	
	public static <T extends Package> T readPackage(PackageIO pacIO, Class<T> packageType) throws ProtocolException {
		Package pac = pacIO.read();
		 try {
			 return packageType.cast(pac);
		 } catch (ClassCastException e) {
			throw new BadPackageException(pac.getClass());
		}
		
	}
}
