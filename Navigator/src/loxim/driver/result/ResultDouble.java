package loxim.driver.result;

public class ResultDouble extends ResultSimple {

	double val;
	public ResultDouble(double _val) {
		val = _val;
	}
	public double getValue() {
		return val;
	}
	
	public String toString() {
		return new Double(val).toString();
	}

	public int getType() {
		return ResultType.RES_DOUBLE;
	}
}
