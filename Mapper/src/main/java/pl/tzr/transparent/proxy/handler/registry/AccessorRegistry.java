package pl.tzr.transparent.proxy.handler.registry;

import java.util.HashMap;
import java.util.Map;

import pl.tzr.transparent.proxy.handler.PropertyAccessor;

public class AccessorRegistry {

	private Map<Class, PropertyAccessor> propertyAccessors =
		new HashMap<Class, PropertyAccessor>();
	
	public void registerHandler(Class clazz, PropertyAccessor handler) {
		propertyAccessors.put(clazz, handler);
	}
	
	public boolean isHandlerAvailable(Class clazz) {
		return propertyAccessors.containsKey(clazz);
	}
	
	public PropertyAccessor getHanlder(Class clazz) {
		return propertyAccessors.get(clazz);
	}
	
}
