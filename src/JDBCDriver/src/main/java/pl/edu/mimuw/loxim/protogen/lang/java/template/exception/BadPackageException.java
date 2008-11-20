package pl.edu.mimuw.loxim.protogen.lang.java.template.exception;

import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class BadPackageException extends ProtocolException {

	private static final long serialVersionUID = -1234556862660937526L;

	private Class<? extends Package> received;
	
	public BadPackageException(Class<? extends Package> received) {
		this.received = received;
	}

	@Override
	public String getMessage() {
		return "Bad package: " + received;
	}
}
