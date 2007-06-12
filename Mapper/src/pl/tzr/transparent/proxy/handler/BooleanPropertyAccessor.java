package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.browser.store.node.SimpleValue.Type;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.exception.InvalidDataStructureException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class BooleanPropertyAccessor 
	extends PrimitivePropertyAccessor<Boolean> 
	implements PropertyAccessor<Boolean> {

	@Override
	protected Boolean fetchPrimitiveValue(Node proxy, PropertyInfo propertyInfo, TransparentSession session) 
		throws SBQLException {
		
		ObjectValue value;
		try {
			value = proxy.getValue();
		} catch (DeletedException e) {
			return null;
		}
		
		if (!(value instanceof SimpleValue)) throw new InvalidDataStructureException();
		SimpleValue simpleValue = (SimpleValue)value;
		
		if (simpleValue.getType() != Type.BOOL) throw new InvalidDataStructureException();
		
		return simpleValue.getBoolean();
	}	

	@Override
	protected ObjectValue createPrimitiveValue(Boolean data, PropertyInfo propertyInfo, TransparentSession session) 
		throws SBQLException, InvalidDataStructureException {
		return new SimpleValue(data.booleanValue());
	}	

}
