package loxim.driver.result;

import java.util.List;

public abstract class Result {
	Result parent = null;
	ResultBase base;	
	public abstract List<Result> getItems();
	public abstract String getLongDescription();
	public void setBase(ResultBase b) {
		if (b != base) { 
			base = b;
			if (getItems() != null)
				for (Result r : getItems()) r.setBase(b);
		}
	}
	public ResultBase getBase() {
		return base;
	}
	
	public void setParent(Result _parent) {
		parent = _parent;
	}
	
	public Result getParent() {
		return parent;
	}
	
	public abstract int getType();
}
