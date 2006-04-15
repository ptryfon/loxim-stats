package szbd.client.result;


public class ResultBool implements Result {
	boolean val;
	public ResultBool(boolean _val) {
		val = _val;
	}
	public boolean getValue() {
		return val;
	}
	
	public String toString() {
		if (val) return "TRUE"; else return "FALSE";
	}
}
