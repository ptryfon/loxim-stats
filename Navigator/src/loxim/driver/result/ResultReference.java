package loxim.driver.result;

public class ResultReference extends ResultSimple {
	String ref;	
	
	public ResultReference(String _ref) {
		ref = _ref;
	}
	
	public String getRef() {
		return ref;
	}
		
	public String getLongDescription() {
		return "REF(" + ref + ")";
	}	
	
	public String toString() {
		return "REF(" + ref + ")";
	}
	
	public int getType() {
		return ResultType.RES_REFERENCE;
	}	
}
