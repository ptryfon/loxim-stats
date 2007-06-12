package pl.tzr.transparent.proxy.handler;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.DeletedException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.proxy.collection.PersistentSet;
import pl.tzr.transparent.structure.model.CollectionPropertyInfo;
import pl.tzr.transparent.structure.model.PropertyInfo;

public class SetPropertyAccessor implements PropertyAccessor<Set> {
	
	public Set retrieveFromBase(
			Node parent, 
			PropertyInfo propertyInfo,
			TransparentSession session)
			throws SBQLException {
		
		CollectionPropertyInfo castedPropertyInfo = 
			(CollectionPropertyInfo)propertyInfo;
		
		
		/* TODO - co gdy zbiór został już pobrany z bazy danych? - jakieś
		 * cache'owanie obiektów
		 */ 
		
		PersistentSet set = new PersistentSet(parent, propertyInfo.getPropertyName(), 
				castedPropertyInfo.getItemClass(), session);
		
		return set;
	}


	public void saveToBase(
			Set data, 
			Node parent,  
			PropertyInfo propertyInfo, 
			TransparentSession session) 
		throws SBQLException, DeletedException {
									
		for (Object item : data) {
			
			Node itemNode = session.getDatabaseContext().getModelRegistry().
				createNodeRepresentation(item, propertyInfo.getPropertyName(), session);
			
			parent.addChild(itemNode);
			
		}				

	}

}
