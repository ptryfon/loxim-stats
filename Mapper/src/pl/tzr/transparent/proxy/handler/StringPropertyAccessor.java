package pl.tzr.transparent.proxy.handler;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.browser.store.node.SimpleValue.Type;
import pl.tzr.driver.loxim.exception.SBQLException;

public class StringPropertyAccessor 
	extends PrimitivePropertyAccessor<String> 
	implements PropertyAccessor<String> {

	@Override
	protected String fetchPrimitiveValue(Node proxy) 
		throws SBQLException, InvalidDataStructure {
		ObjectValue value = proxy.getValue();		
		if (!(value instanceof SimpleValue)) throw new InvalidDataStructure();
		SimpleValue simpleValue = (SimpleValue)value;
		
		if (simpleValue.getType() != Type.STRING) throw new InvalidDataStructure();
		
		return simpleValue.getString();
	}

	@Override
	protected ObjectValue createPimitiveValue(String data) 
		throws SBQLException, InvalidDataStructure {

		return new SimpleValue(data);
	}
	

}
