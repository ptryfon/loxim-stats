package pl.edu.mimuw.loxim.data;

import java.util.ArrayList;
import java.util.Collection;

public class GenericCollection implements Struct, Sequence, Bag {

	private Collection<Object> data = new ArrayList<Object>();
	private Class<?> globalType;
	
	@Override
	public Collection<Object> getData() {
		return data;
	}

	@Override
	public Class<?> getGlobalType() {
		return globalType;
	}

	@Override
	public void setGlobalType(Class<?> globalType) {
		this.globalType = globalType;
	}

	@Override
	public <T> T mapTo(Class<T> clazz) throws IllegalArgumentException {
		// TODO Auto-generated method stub
		return null;
	}

}
