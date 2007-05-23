package pl.tzr.transparent.structure.model;

import pl.tzr.transparent.proxy.handler.PropertyAccessor;

public class CollectionPropertyInfo extends PropertyInfo {

	private final Class itemClass;
	
	public CollectionPropertyInfo(final String propertyName, final Class clazz, final Class itemClass, final PropertyAccessor typeHandler) {
		super(propertyName, clazz, typeHandler);
		this.itemClass = itemClass;
	}

	public Class getItemClass() {
		return itemClass;
	}	
	
}
