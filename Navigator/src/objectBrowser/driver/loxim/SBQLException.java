package objectBrowser.driver.loxim;

public class SBQLException extends Exception {
	public static final long ERR_NESTED = 0;
	long errorCode;
	
	public String getModuleCode() {
		return Integer.toHexString((int)(errorCode >> 20));
	}
	
	public String getDetailCode() {
		return Integer.toHexString((int)errorCode & 0xFFFFF);
	}
	
	

	public SBQLException(long _errorCode) {		
		super();
		errorCode = _errorCode;
	}	
	
	public SBQLException(String message, Throwable t) {
		super(message, t);
		errorCode = ERR_NESTED;		
	}
	
	public long getErrorCode() {
		return errorCode;
	}

	@Override
	public String getMessage() {
		return "Execution error (module:" + getModuleCode() + " errno:" + getDetailCode() + ")";
	}
}
