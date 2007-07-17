package pl.tzr.driver.loxim.result;


public class ResultString extends ResultSimple {
	String val;
	public ResultString(String _val) {
		val = _val;
	}
	
	public String getValue() {
		return val;
	}
	
    @Override
	public String toString() {
		return "\"" + val + "\""; 
	}
	
    @Override
	public int getType() {
		return ResultType.RES_STRING;
	}	
}
