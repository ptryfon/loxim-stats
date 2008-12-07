package pl.edu.mimuw.loxim.data;

public class VoidImpl implements Void {

	@Override
	public <T> T mapTo(Class<T> clazz) throws IllegalArgumentException {
		return null;
	}

}
