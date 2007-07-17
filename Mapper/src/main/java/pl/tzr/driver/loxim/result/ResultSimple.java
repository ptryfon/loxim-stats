package pl.tzr.driver.loxim.result;

import java.util.List;

public abstract class ResultSimple extends Result {

	@Override
    public List<Result> getItems() {
		return null;
	}

	@Override
    public String getLongDescription() {
		return this.toString();
	}
}
