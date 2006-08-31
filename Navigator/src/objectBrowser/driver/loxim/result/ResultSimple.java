package objectBrowser.driver.loxim.result;

import java.util.List;
import java.util.Observable;

public abstract class ResultSimple extends Result {

	public List<Result> getItems() {
		return null;
	}

	public String getLongDescription() {
		return this.toString();
	}
}
