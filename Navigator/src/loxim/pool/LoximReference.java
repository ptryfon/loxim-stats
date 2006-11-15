package loxim.pool;

import loxim.driver.exception.SBQLException;

public class LoximReference extends LoximObject {
	private String targetReference;
	private LoximObject target;
	
	public LoximReference(String targetReference) {
		this.targetReference = targetReference;
		this.target = null;
	}
	
	public void fetchTarget(ConnectionService service) throws SBQLException {
		target = service.fetchReferencedObject(this.targetReference);
	}
	
	public LoximObject getTarget() {
		return target;
	}
	
	public String toString2() {
		return "@" + targetReference;
	}
	
	public String getTargetRef() {
		return targetReference;
	}
}
