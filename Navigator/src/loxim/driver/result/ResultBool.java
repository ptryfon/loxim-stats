package loxim.driver.result;


public class ResultBool extends ResultSimple {
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
	
	public int getType() {
		if (val) return ResultType.RES_BOOLTRUE; else return ResultType.RES_BOOLFALSE;
	}
}
