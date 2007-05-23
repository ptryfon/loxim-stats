package pl.tzr.transparent.proxy.handler;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.transparent.TransparentProxyFactory;
import pl.tzr.transparent.proxy.collection.PersistentSet;
import pl.tzr.transparent.structure.model.CollectionPropertyInfo;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class SetPropertyAccessor implements PropertyAccessor<Set> {
	
	public Set retrieveFromBase(Node parent, String propertyName,
			PropertyInfo propertyInfo,
			TransparentProxyFactory transparentProxyFactory)
			throws SBQLException {
		
		CollectionPropertyInfo castedPropertyInfo = 
			(CollectionPropertyInfo)propertyInfo;
		
		
		/* TODO - co gdy zbiór został już pobrany z bazy danych? - jakieś
		 * cache'owanie obiektów
		 */ 
		
		PersistentSet set = new PersistentSet(parent, propertyName, 
				castedPropertyInfo.getItemClass(), transparentProxyFactory);
		
		return set;
	}


	public void saveToBase(Set data, Node parent, String propertyName, 
			PropertyInfo propertyInfo) throws SBQLException {

		throw new UnsupportedOperationException();
	}

}
