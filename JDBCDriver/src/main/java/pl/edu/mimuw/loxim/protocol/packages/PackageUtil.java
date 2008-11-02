package pl.edu.mimuw.loxim.protocol.packages;

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
	
}
