package pl.tzr.driver.loxim.exception;

import java.io.IOException;
/**
 * Exception thrown in case of errors during communication with the database
 * (nests IOException) 
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class SBQLIOException extends SBQLException {
	private static final long serialVersionUID = 1L;
	
	protected IOException exception;
	
	/**
	 * Zwraca zagniezdzony wyjatek klasy IOException
	 * @return zagniezdzony wyjatek
	 */
	public IOException getNested() {
		return exception;
	}
	
	public SBQLIOException(IOException exception) {
		super();
		this.exception = exception;
	}
	
	@Override
	public String getMessage() {
		return "IOException:" + exception;
	}	
}