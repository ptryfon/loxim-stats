package pl.tzr.driver.loxim.result;

import java.util.Observable;

public class ResultBase extends Observable {
	Result root;
	public ResultBase(Result _root) {
		root = _root;
		root.setBase(this);
	}
	
	public void notifyChanged(Result r) {
		setChanged();
		notifyObservers(r);
	}
	
	public Result getRoot() {
		return root;
	}
}
