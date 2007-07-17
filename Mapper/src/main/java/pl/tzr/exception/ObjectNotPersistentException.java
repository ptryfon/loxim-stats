package pl.tzr.exception;

/**
 * Exception thrown when user tries to execute operation on transient object
 * while the operation requires persistent object.
 * @author Tomasz Rosiek
 *
 */
public class ObjectNotPersistentException extends MappingException {

    private static final long serialVersionUID = 1L;
    
}
