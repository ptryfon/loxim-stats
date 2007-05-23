package pl.tzr.transparent.structure.model;

import pl.tzr.transparent.proxy.handler.PropertyAccessor;

public class PropertyInfo {
		
	private final String propertyName;
	
	private final Class clazz;

	private final PropertyAccessor typeHandler;
	
	public PropertyInfo(final String propertyName, final Class clazz, final PropertyAccessor typeHandler) {
		super();
		this.propertyName = propertyName;
		this.clazz = clazz;
		this.typeHandler = typeHandler;
	}	

	public Class getClazz() {
		return clazz;
	}

	public PropertyAccessor getTypeHandler() {
		return typeHandler;
	}

	public String getPropertyName() {
		return propertyName;
	}

	
}
