package objectBrowser.model;

public class ReferenceDataObject extends DataObject {
	protected String targetOid;
	
	public ReferenceDataObject(String name, String targetOid) {
		super(name);
		this.targetOid = targetOid;
	}

	public String getTargetOid() {
		return targetOid;
	}

	public void setTargetOid(String targetOid) {
		this.targetOid = targetOid;
	}
}
