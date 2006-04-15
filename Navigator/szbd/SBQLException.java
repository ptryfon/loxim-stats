package szbd;

public class SBQLException extends Exception {
	public static final long ERR_NESTED = 0;
	long errorCode;
	public SBQLException(long _errorCode, String message) {
		super(message);
		errorCode = _errorCode;		
	}
	
	public SBQLException(String message, Throwable t) {
		super(message, t);
		errorCode = ERR_NESTED;		
	}
	
	public long getErrorCode() {
		return errorCode;
	}
}
