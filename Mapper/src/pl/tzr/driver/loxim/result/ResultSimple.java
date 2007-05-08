package pl.tzr.driver.loxim.result;

import java.util.List;

public abstract class ResultSimple extends Result {

	public List<Result> getItems() {
		return null;
	}

	public String getLongDescription() {
		return this.toString();
	}
}
