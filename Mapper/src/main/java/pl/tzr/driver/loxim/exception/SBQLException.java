package pl.tzr.driver.loxim.exception;

/**
 * Abstract class representing exception in interface to LoXiM database
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public abstract class SBQLException extends Exception {
	public SBQLException() {
		super();
	}

	protected SBQLException(String message) {
		super(message);
	}
}
