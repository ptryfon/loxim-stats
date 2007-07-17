package pl.tzr.driver.loxim.exception;

/**
 * Exception thrown when the error occurred on the server side
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class SBQLServerException extends SBQLException {
	
	private static final long serialVersionUID = 1L;
    
	long errorCode;
    
    public SBQLServerException(long _errorCode) {       
        super();
        errorCode = _errorCode;
    }   
	
	/**
     * Returns the number of a module where the error has occurred
     * (see sources of the database)
	 * @return module number
	 */
	public String getModuleCode() {
		return Integer.toHexString((int)(errorCode >> 20));
	}
	
	/**
     * Returns the error code (see sources of the database)
	 * @return the error code
	 */
	public String getDetailCode() {
		return Integer.toHexString((int)errorCode & 0xFFFFF);
	}
		
	public long getErrorCode() {
		return errorCode;
	}

	@Override
	public String getMessage() {
		return "Server execution error (module:" + getModuleCode() + " errno:" + getDetailCode() + ")";
	}	
}
