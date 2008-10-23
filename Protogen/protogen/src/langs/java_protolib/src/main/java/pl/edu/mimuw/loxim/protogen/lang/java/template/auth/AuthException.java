package pl.edu.mimuw.loxim.protogen.lang.java.template.auth;

public class AuthException extends Exception {

	private static final long serialVersionUID = -5262130742914713006L;

	public AuthException() {
		super();
	}

	public AuthException(String message, Throwable cause) {
		super(message, cause);
	}

	public AuthException(String message) {
		super(message);
	}

	public AuthException(Throwable cause) {
		super(cause);
	}

}
