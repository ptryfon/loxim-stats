package pl.edu.mimuw.loxim.data;

public interface LoXiMObject {
	
	public <T> T mapTo(Class<T> clazz) throws IllegalArgumentException;
	
}
