package pl.tzr.transparent.structure.model;

import pl.tzr.transparent.proxy.handler.PropertyAccessor;

public class PropertyInfo<T> {
		
	private final String propertyName;
	
	private final Class<T> clazz;

	private final PropertyAccessor<T> typeHandler;
	
	public PropertyInfo(final String propertyName, final Class<T> clazz, final PropertyAccessor<T> typeHandler) {
		super();
		this.propertyName = propertyName;
		this.clazz = clazz;
		this.typeHandler = typeHandler;
	}	

	public Class<T> getClazz() {
		return clazz;
	}

	public PropertyAccessor<T> getTypeHandler() {
		return typeHandler;
	}

	public String getPropertyName() {
		return propertyName;
	}

	
}
