package pl.tzr.driver.loxim.result;


public class ResultVoid extends ResultSimple {
	public String toString() {
		return "VOID";
	}
	
	public int getType() {
		return ResultType.RES_VOID;
	}	
}
