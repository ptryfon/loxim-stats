package pl.tzr.exception;

/**
 * Superclass for all exceptions of the mapping layer
 * @author Tomasz Rosiek
 *
 */
public class MappingException extends RuntimeException {
	
	public MappingException() {
		super();
	}
	
	protected MappingException(Exception e) {
		super(e);
	}

}
