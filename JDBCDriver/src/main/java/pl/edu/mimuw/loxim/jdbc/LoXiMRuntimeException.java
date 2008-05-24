package pl.edu.mimuw.loxim.jdbc;

public class LoXiMRuntimeException extends RuntimeException {

	private static final long serialVersionUID = 7334049155775735166L;

	public LoXiMRuntimeException() {

	}

	public LoXiMRuntimeException(String message) {
		super(message);
	}

	public LoXiMRuntimeException(Throwable cause) {
		super(cause);
	}

	public LoXiMRuntimeException(String message, Throwable cause) {
		super(message, cause);
	}
}
