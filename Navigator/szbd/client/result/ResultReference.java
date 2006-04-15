package szbd.client.result;


public class ResultReference implements Result {
	String ref;
	public ResultReference(String _ref) {
		ref = _ref;
	}
	
	public String getRef() {
		return ref;
	}
	
	public String toString() {
		return "REF(" + ref + ")";
	}
}
