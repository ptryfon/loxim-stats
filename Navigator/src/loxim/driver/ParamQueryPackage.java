package loxim.driver;

class ParamQueryPackage extends SimpleQueryPackage {
	
	public ParamQueryPackage() {
		
	}
	
	public ParamQueryPackage(String query) {
		super(query);
	}
	
	public short getType() {
		return Package.TYPE_PARAMQUERY;
	}
}
