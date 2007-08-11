package pl.tzr.exception;

/**
 * Exception thrown when user tries to execute an invalid operation on
 * mapped object
 * @author Tomasz Rosiek
 *
 */
public class InvalidOperationException extends MappingException {

    private static final long serialVersionUID = 1L;

    public InvalidOperationException(String msg) {
        super(msg);
    }
    
}
