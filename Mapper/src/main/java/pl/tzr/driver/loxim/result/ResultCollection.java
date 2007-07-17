package pl.tzr.driver.loxim.result;

import java.util.List;



public abstract class ResultCollection extends Result {
	@Override
    public abstract List<Result> getItems();
	public void addItem(Result r) {
		getItems().add(r);
		r.setParent(this);
	}
}
