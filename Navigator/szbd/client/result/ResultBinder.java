package szbd.client.result;


public class ResultBinder implements Result {
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
	
	public String toString() {
		return "BINDER(key=" + key + "value=" + value + ")";
	}
}
