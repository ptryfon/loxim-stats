package loxim.pool;

import loxim.driver.exception.SBQLProtocolException;
import loxim.driver.result.Result;
import loxim.driver.result.ResultBinder;
import loxim.driver.result.ResultReference;
import loxim.driver.result.ResultStruct;


/**
 * Fabryka tworząca obiekty na podstawie wyników zapytań
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class LoximObjectFactory {
	/**
	 * Tworzy obiekt znajdujacy sie w wyniku zapytania bazodanowego
	 * @param result
	 * @return
	 */
	public LoximObject buildObject(Result result) throws SBQLProtocolException {
		if (result instanceof ResultStruct) {
			return buildComposite((ResultStruct)result);
		} else if (result instanceof ResultBinder) {
			return buildBinder((ResultBinder)result);
		} else if (result instanceof ResultReference) {
			return buildReference((ResultReference)result);
		} else {
			return buildSimple(result);
		}
	}
	
	protected LoximObject buildBinder(ResultBinder result) throws SBQLProtocolException {
		LoximObject object = buildObject(result.getValue());
		object.setName(result.getKey());
		return object;
	}
	
	protected LoximObject buildSimple(Result result) {
		return new LoximSimpleObject(result.getLongDescription());
	}
	
	protected LoximObject buildComposite(ResultStruct result) throws SBQLProtocolException {
		LoximCompositeObject object = new LoximCompositeObject();
		for (Result item : result.getItems()) {
			LoximObject loximItem = buildObject(item);
			object.getProperties().add(loximItem);
		}
		
		return object;
	}
	
	protected LoximObject buildReference(ResultReference result) {
		return new LoximReference(result.getRef());
	}
}
