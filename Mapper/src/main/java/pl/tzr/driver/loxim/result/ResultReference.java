package pl.tzr.driver.loxim.result;

public class ResultReference extends ResultSimple {
	String ref;	
	
	public ResultReference(String _ref) {
		ref = _ref;
	}
	
	public String getRef() {
		return ref;
	}
		
    @Override
	public String getLongDescription() {
		return "REF(" + ref + ")";
	}	
	
    @Override
	public String toString() {
		return "REF(" + ref + ")";
	}
	
    @Override
	public int getType() {
		return ResultType.RES_REFERENCE;
	}	
}
