package objectBrowser.driver.loxim.result;


public class ResultString extends ResultSimple {
	String val;
	public ResultString(String _val) {
		val = _val;
	}
	
	public String getValue() {
		return val;
	}
	
	public String toString() {
		return "\"" + val + "\""; 
	}
}
