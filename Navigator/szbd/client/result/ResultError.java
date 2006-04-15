package szbd.client.result;


public class ResultError implements Result {
	long errorCode;
	public ResultError(long _errorCode) {
		errorCode = _errorCode;
	}
	
	public long getErrorCode() {
		return errorCode;
	}
	
	public String toString() {
		return "ERROR(" + errorCode + ")";
	}
}
