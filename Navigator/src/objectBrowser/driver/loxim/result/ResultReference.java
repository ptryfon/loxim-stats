package objectBrowser.driver.loxim.result;

import java.util.ArrayList;
import java.util.List;


public class ResultReference extends ResultCollection {
	String ref;
	Result target = null;	
	
	public ResultReference(String _ref) {
		ref = _ref;
	}
	
	public String getRef() {
		return ref;
	}
	
	public Result getTarget() {
		return target;
	}
	
	public void bindTarget(Result _target) {
		target = _target;
		/* TO MO¯E BYÆ NIEPOTRZEBNE PÓ¬NIEJ */
		target.setBase(base);
		target.setParent(this);
		base.notifyChanged(this);
	}
	
	public void unbindTarget() {
		target = null;
		base.notifyChanged(this);
	}
	
	public List<Result> getItems() {
		if (target != null) {
			List<Result> l = new ArrayList<Result>(1);
			l.add(target);
			return l;
		} else return null;
	}	
	
	public String getLongDescription() {
		return "REF(" + ref + ")";
	}	
	
	public String toString() {
		return "REF(" + ref + ")";
	}
}
