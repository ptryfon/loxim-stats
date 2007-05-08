package pl.tzr.driver.loxim.result;

import java.util.ArrayList;
import java.util.List;


public class ResultBinder extends ResultCollection {
	String key;
	Result value;
	public ResultBinder(String _key, Result _value) {
		key = _key;
		value = _value;
	}
	
	public String getKey() {
		return key;
	}
	
	public Result getValue() {
		return value;
	}
	
	public List<Result> getItems() {
		List<Result> l = new ArrayList<Result>(1);
		l.add(value);
		return l;
	}
	
	public String toString() {
		return "BINDER(key=" + key + ")";
	}
	
	public String getLongDescription() {
		return "BINDER(key=" + key + "value=" + value + ")";
	}
	
	public int getType() {
		return ResultType.RES_BINDER;
	}
}
