package pl.edu.mimuw.loxim.data;

public interface Binding extends LoXiMObject {

	public String getBindingName();
	public void setBindingName(String bindingName);
	public Object getValue();
	public void setValue(Object value);
	
}
