package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.browser.store.node.SimpleValue.Type;
import pl.tzr.driver.loxim.exception.SBQLException;

public class BooleanPropertyAccessor 
	extends PrimitivePropertyAccessor<Boolean> 
	implements PropertyAccessor<Boolean> {

	@Override
	protected Boolean fetchPrimitiveValue(Node proxy) 
		throws SBQLException, InvalidDataStructure {
		ObjectValue value = proxy.getValue();		
		if (!(value instanceof SimpleValue)) throw new InvalidDataStructure();
		SimpleValue simpleValue = (SimpleValue)value;
		
		if (simpleValue.getType() != Type.BOOL) throw new InvalidDataStructure();
		
		return simpleValue.getBoolean();
	}	

	@Override
	protected ObjectValue createPimitiveValue(Boolean data) 
		throws SBQLException, InvalidDataStructure {
		return new SimpleValue(data.booleanValue());
	}	

}
