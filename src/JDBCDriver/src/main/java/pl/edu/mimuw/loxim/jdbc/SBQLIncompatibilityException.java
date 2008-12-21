package pl.edu.mimuw.loxim.jdbc;

import java.sql.SQLException;

public class SBQLIncompatibilityException extends SQLException {

	private static final long serialVersionUID = 4480199372797842374L;

	public SBQLIncompatibilityException() {

	}

	public SBQLIncompatibilityException(String reason) {
		super(reason);
	}

	public SBQLIncompatibilityException(Throwable cause) {
		super(cause);
	}

	public SBQLIncompatibilityException(String reason, String SQLState) {
		super(reason, SQLState);
	}

	public SBQLIncompatibilityException(String reason, Throwable cause) {
		super(reason, cause);
	}

	public SBQLIncompatibilityException(String reason, String SQLState, int vendorCode) {
		super(reason, SQLState, vendorCode);
	}

	public SBQLIncompatibilityException(String reason, String sqlState, Throwable cause) {
		super(reason, sqlState, cause);
	}

	public SBQLIncompatibilityException(String reason, String sqlState, int vendorCode, Throwable cause) {
		super(reason, sqlState, vendorCode, cause);
	}

}
