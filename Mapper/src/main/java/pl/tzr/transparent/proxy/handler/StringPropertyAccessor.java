package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.browser.store.node.SimpleValue.Type;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class StringPropertyAccessor 
	extends PrimitivePropertyAccessor<String> 
	implements PropertyAccessor<String> {

	@Override
	protected String fetchPrimitiveValue(
            Node proxy, 
            PropertyInfo propertyInfo, 
            TransparentSession session) {
		
		ObjectValue value;
		try {
			value = proxy.getValue();
		} catch (DeletedException e) {
			return null;
		}
		
		if (!(value instanceof SimpleValue)) 
			throw new InvalidDataStructureException();
		
		SimpleValue simpleValue = (SimpleValue)value;
		
		if (simpleValue.getType() != Type.STRING) 
			throw new InvalidDataStructureException();
		
		return simpleValue.getString();
	}

	@Override
	protected ObjectValue createPrimitiveValue(
            String data, 
            PropertyInfo propertyInfo, 
            TransparentSession session) {

		return new SimpleValue(data);
	}
	

}