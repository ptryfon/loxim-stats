package szbd.client.result;


public class ResultInt implements Result {
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
