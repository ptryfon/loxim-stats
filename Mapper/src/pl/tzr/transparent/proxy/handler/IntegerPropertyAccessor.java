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

public class IntegerPropertyAccessor 
	extends PrimitivePropertyAccessor<Integer> 
	implements PropertyAccessor<Integer> {

	@Override
	protected Integer fetchPrimitiveValue(Node proxy, PropertyInfo propertyInfo, TransparentSession session) 
		throws SBQLException {
		
		ObjectValue value;
		try {
			value = proxy.getValue();
		} catch (DeletedException e) {
			return null;
		}		
		
		if (!(value instanceof SimpleValue)) throw new InvalidDataStructureException();
		SimpleValue simpleValue = (SimpleValue)value;
		
		if (simpleValue.getType() != Type.INT) throw new InvalidDataStructureException();
		
		return simpleValue.getInteger();
	}	

	@Override
	protected ObjectValue createPrimitiveValue(Integer data, PropertyInfo propertyInfo, TransparentSession session) 
		throws SBQLException {
		return new SimpleValue(data.intValue());
	}
	

}
