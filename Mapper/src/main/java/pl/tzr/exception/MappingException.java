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
    
    public MappingException(String msg, Throwable t) {
        super(msg, t);
    }

    public MappingException(String msg) {
        super(msg);
    }

    public MappingException(Throwable t) {
        super(t);
    }

    private static final long serialVersionUID = 1L;    
	
	

}
