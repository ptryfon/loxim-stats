package pl.tzr.transparent.structure.model;

import pl.tzr.transparent.proxy.handler.PropertyAccessor;

/**
 * Subclass of PropertyInfo class wich can contain information about
 * the way of mapping a collection
 * @author Tomasz Rosiek
 *
 */
public class CollectionPropertyInfo extends PropertyInfo {

	private final Class itemClass;
	
	public CollectionPropertyInfo(
			final String propertyName,
            final String nodeName,
			final Class clazz, 
			final Class itemClass, 
			final PropertyAccessor typeHandler) {
		
		super(propertyName, nodeName, clazz, typeHandler);
		this.itemClass = itemClass;
	}

	/**
	 * Returns a class of objects wich mapped collection could contain
	 * @return
	 */
	public Class getItemClass() {
		return itemClass;
	}	
	
	@Override
	public boolean isValueCacheable() {
		return true;
	}
	
}
