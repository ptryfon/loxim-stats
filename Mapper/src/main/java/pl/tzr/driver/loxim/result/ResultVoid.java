package pl.tzr.driver.loxim.result;


public class ResultVoid extends ResultSimple {
    
    @Override
	public String toString() {
		return "VOID";
	}
	
    @Override
	public int getType() {
		return ResultType.RES_VOID;
	}	
}
