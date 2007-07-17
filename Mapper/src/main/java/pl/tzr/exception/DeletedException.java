package pl.tzr.exception;

/**
 * Thrown when user tries to fetch data from Node representing object
 * that has been deleted.
 * @author Tomasz Rosiek
 *
 */
public class DeletedException extends RuntimeException {
    
    private static final long serialVersionUID = 1L;    
	
	private final String ref;

	public DeletedException(final String ref) {
		this.ref = ref;
	}

	public String getRef() {
		return ref;
	}
	
}
