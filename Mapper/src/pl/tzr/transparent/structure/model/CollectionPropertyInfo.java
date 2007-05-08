package pl.tzr.transparent.structure.model;

import pl.tzr.transparent.proxy.handler.PropertyAccessor;

public class CollectionPropertyInfo<T> extends PropertyInfo<T> {

	private final Class itemClass;
	
	public CollectionPropertyInfo(final String propertyName, final Class<T> clazz, final Class itemClass, final PropertyAccessor<T> typeHandler) {
		super(propertyName, clazz, typeHandler);
		this.itemClass = itemClass;
	}

	public Class getItemClass() {
		return itemClass;
	}	
	
}
