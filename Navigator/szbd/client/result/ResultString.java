package szbd.client.result;


public class ResultString implements Result {
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
