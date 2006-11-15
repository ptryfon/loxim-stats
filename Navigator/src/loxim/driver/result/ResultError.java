package loxim.driver.result;


public class ResultError extends ResultSimple {
	long errorCode;
	public ResultError(long _errorCode) {
		errorCode = _errorCode;
	}
	
	public long getErrorCode() {
		return errorCode;
	}
	
	public String toString() {
		return "ERROR(" + Long.toHexString(errorCode) + ")";
	}
	
	public int getType() {
		return ResultType.RES_ERROR;
	}	
}
