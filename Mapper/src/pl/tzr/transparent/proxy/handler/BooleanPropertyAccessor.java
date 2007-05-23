package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.browser.store.node.SimpleValue.Type;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.InvalidDataStructureException;

public class BooleanPropertyAccessor 
	extends PrimitivePropertyAccessor<Boolean> 
	implements PropertyAccessor<Boolean> {

	@Override
	protected Boolean fetchPrimitiveValue(Node proxy) 
		throws SBQLException {
		ObjectValue value = proxy.getValue();		
		if (!(value instanceof SimpleValue)) throw new InvalidDataStructureException();
		SimpleValue simpleValue = (SimpleValue)value;
		
		if (simpleValue.getType() != Type.BOOL) throw new InvalidDataStructureException();
		
		return simpleValue.getBoolean();
	}	

	@Override
	protected ObjectValue createPimitiveValue(Boolean data) 
		throws SBQLException, InvalidDataStructureException {
		return new SimpleValue(data.booleanValue());
	}	

}
