package pl.tzr.transparent.proxy.handler.registry;

import pl.tzr.transparent.proxy.handler.BooleanPropertyAccessor;
import pl.tzr.transparent.proxy.handler.IntegerPropertyAccessor;
import pl.tzr.transparent.proxy.handler.StringPropertyAccessor;

public class SimpleAccessorRegistryFactory implements AccessorRegistryFactory {

	public AccessorRegistry getHandlerRegistry() {
		
		AccessorRegistry registry = new AccessorRegistry();
		
		IntegerPropertyAccessor integerPropertyAccessor = new IntegerPropertyAccessor();
		
		registry.registerHandler(int.class, integerPropertyAccessor);
		registry.registerHandler(Integer.class, integerPropertyAccessor);

		BooleanPropertyAccessor booleanPropertyAccessor = new BooleanPropertyAccessor();
		
		registry.registerHandler(boolean.class, booleanPropertyAccessor);
		registry.registerHandler(Boolean.class, booleanPropertyAccessor);
		
		StringPropertyAccessor stringPropertyAccessor = new StringPropertyAccessor();
		
		registry.registerHandler(String.class, stringPropertyAccessor);

		return registry;
	}

}
