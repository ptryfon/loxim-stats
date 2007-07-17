package pl.tzr.transparent.structure.model;

import pl.tzr.transparent.proxy.handler.PropertyAccessor;

public class PropertyInfo {
		
	private final String propertyName;
    
    private final String nodeName;
	
	private final Class clazz;

	private final PropertyAccessor accessor;
	
	public PropertyInfo(
			final String propertyName,
            final String nodeName,
			final Class clazz, 
			final PropertyAccessor accessor) {
		
		this.propertyName = propertyName;
        this.nodeName = nodeName;
		this.clazz = clazz;
		this.accessor = accessor;
	}	

	public Class getClazz() {
		return clazz;
	}

	public PropertyAccessor getAccessor() {
		return accessor;
	}

	public String getPropertyName() {
		return propertyName;
	}
	
	public boolean isValueCacheable() {
		return false;
	}

    public String getNodeName() {
        return nodeName;
    }

	
}
