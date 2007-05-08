package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.browser.store.node.SimpleValue.Type;
import pl.tzr.driver.loxim.exception.SBQLException;

public class IntegerPropertyAccessor 
	extends PrimitivePropertyAccessor<Integer> 
	implements PropertyAccessor<Integer> {

	@Override
	protected Integer fetchPrimitiveValue(Node proxy) 
		throws SBQLException, InvalidDataStructure {
		ObjectValue value = proxy.getValue();		
		if (!(value instanceof SimpleValue)) throw new InvalidDataStructure();
		SimpleValue simpleValue = (SimpleValue)value;
		
		if (simpleValue.getType() != Type.INT) throw new InvalidDataStructure();
		
		return simpleValue.getInteger();
	}	

	@Override
	protected ObjectValue createPimitiveValue(Integer data) 
		throws SBQLException, InvalidDataStructure {
		return new SimpleValue(data.intValue());
	}
	

}
