package pl.tzr.transparent.structure.model;

import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.beanutils.PropertyUtilsBean;

import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.MappingException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.transparent.TransparentSession;


/**
 * Represents the data model and mapping definition. Contains the list of
 * all classes wich could be mapped to the LoXiM database with the
 * way of mapping
 * @author Tomasz Rosiek
 *
 */
public class ModelRegistry {
	
	private final PropertyUtilsBean propertyUtilsBean = new PropertyUtilsBean();

	private Map<Class, ClassInfo> registry = new HashMap<Class, ClassInfo>();
	
	private Map<String, ClassInfo> registryByName = new HashMap<String, ClassInfo>();
	
	public void registerClass(ClassInfo classInfo) {
		
		registry.put(classInfo.getClazz(), classInfo);
		registryByName.put(classInfo.getEntityName(), classInfo);
		
	}
	
    @SuppressWarnings("unchecked")
	public ClassInfo getClassInfo(Class clazz) {
		for (Entry<Class, ClassInfo> registeredClass : registry.entrySet()) {
			
			if (registeredClass.getKey().isAssignableFrom(clazz)) {
				
				return registeredClass.getValue();
			}
			
		}
		
		throw new IllegalStateException(
				"There is no mapping definition for class "  + clazz.getName());
		
	}
	
	public Node createNodeRepresentation(Object object, 
			TransparentSession session) {
		
		ClassInfo classInfo = getClassInfo(object.getClass());
		
		return createNodeRepresentation(
				object, classInfo.getEntityName(), session);
		
	}	
	
    @SuppressWarnings("unchecked")
	public Node createNodeRepresentation(
			Object object, 
			String name, 
			TransparentSession session) {
		
		ClassInfo classInfo = getClassInfo(object.getClass());
		
		Node node = new LoximNode(name, new ComplexValue());
		
		for (PropertyInfo propertyInfo : classInfo.getProperties()) {
			
			try {
				
				Object propertyValue = propertyUtilsBean.getProperty(
						object, propertyInfo.getPropertyName());
								
				propertyInfo.getAccessor().saveToBase(
						propertyValue, node, propertyInfo, session);
				
			} catch (SBQLException e) {
				throw new NestedSBQLException(e);
			} catch (InvocationTargetException e) {
				//FIXME provide more information to the mapping exception
				throw new MappingException();
			} catch (IllegalAccessException e) {
//				FIXME provide more information to the mapping exception
				throw new MappingException();
			} catch (NoSuchMethodException e) {
//				FIXME provide more information to the mapping exception
				throw new MappingException();				
			}
		}
		
		return node;
	}	
	
	public ClassInfo getClassInfo(String entityName) {
		return registryByName.get(entityName);
	}
	
}
