package pl.tzr.exception;

/**
 * Thrown when user tries to fetch data from Node representing object
 * that has been deleted.
 * @author Tomasz Rosiek
 *
 */
public class DeletedException extends Exception {
	
	private final String ref;

	public DeletedException(final String ref) {
		this.ref = ref;
	}

	public String getRef() {
		return ref;
	}
	
}
