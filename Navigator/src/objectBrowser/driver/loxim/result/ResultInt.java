package objectBrowser.driver.loxim.result;


public class ResultInt extends ResultSimple {
	int val;
	public ResultInt(int _val) {
		val = _val;
	}
	
	public int getValue() {
		return val;
	}
	
	public String toString() {
		return "" + val;
	}
}
