package pl.edu.mimuw.loxim.data;

public class BindingImpl implements Binding {

	private String bindingName;
	private Object value;
	
	@Override
	public String getBindingName() {
		return bindingName;
	}

	@Override
	public Object getValue() {
		return value;
	}

	@Override
	public void setBindingName(String bindingName) {
		this.bindingName = bindingName;
	}

	@Override
	public void setValue(Object value) {
		this.value = value;
	}

}
