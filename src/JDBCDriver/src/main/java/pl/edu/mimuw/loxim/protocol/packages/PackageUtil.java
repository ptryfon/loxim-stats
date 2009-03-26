package pl.edu.mimuw.loxim.protocol.packages;

import pl.edu.mimuw.loxim.protocol.enums.ErrorsEnum;


public final class PackageUtil {

	public static String toString(A_sc_errorPackage errorPac) {
		String msg = errorPac.getDescription();
		StringBuilder buf = new StringBuilder(msg.length() + 20);
		buf.append("ERROR ");
		ErrorsEnum error = errorPac.getError_code();
		if (error != null) {
			buf.append("#");
			buf.append(error.getValue());
			buf.append(" (");
			buf.append(error);
			buf.append(") ");
		}
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
		buf.append(msg);
		return buf.toString();
	}
}
