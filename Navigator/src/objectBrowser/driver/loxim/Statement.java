package objectBrowser.driver.loxim;

import java.util.HashMap;
import java.util.Map;

import objectBrowser.driver.loxim.result.Result;

public class Statement {
	private long number;
	private Map<String, Result> params = new HashMap<String, Result>();
	
	public Statement(long number) {
		this.number = number;
	}

	public long getNumber() {
		return number;
	}
	
	public void setParam(String name, Result value) {
		params.put(name, value);
	}
	
	public Map<String, Result> getParams() {
		return params;
	}
}
