package pl.tzr.driver.loxim.result;


public class ResultError extends ResultSimple {
	long errorCode;
	public ResultError(long _errorCode) {
		errorCode = _errorCode;
	}
	
	public long getErrorCode() {
		return errorCode;
	}
	
	@Override
    public String toString() {
		return "ERROR(" + Long.toHexString(errorCode) + ")";
	}
	
	@Override
    public int getType() {
		return ResultType.RES_ERROR;
	}	
}
