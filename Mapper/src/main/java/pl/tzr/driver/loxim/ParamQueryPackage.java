package pl.tzr.driver.loxim;

class ParamQueryPackage extends SimpleQueryPackage {
	
	public ParamQueryPackage() {
		/* Default constructor */
	}
	
	public ParamQueryPackage(String query) {
		super(query);
	}
	
	@Override
    public short getType() {
		return Package.TYPE_PARAMQUERY;
	}
}
