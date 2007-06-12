package pl.tzr.transparent.structure.model;

import java.util.Collection;
import java.util.Map;


/** 
 * Stores information how objects of the specified Java class are
 * mapped in LoXiM database
 *   
 * @author Tomasz Rosiek
 *
 */
public class ClassInfo {
	
	private final String entityName;
	private final Class clazz;
	private final Map<String, PropertyInfo> properties;		
	
	
	public ClassInfo(
			final String entityName, 
			final Class clazz, 
			final Map<String, PropertyInfo> properties) {
		super();
		this.entityName = entityName;
		this.clazz = clazz;
		this.properties = properties;
	}

	/**
	 * Returns a name of LoXiM entity wich will be used when we store
	 * Java object
	 * @return
	 */
	public String getEntityName() {
		return entityName;
	}

	/**
	 * Returns mapped Java class
	 * @return
	 */
	public Class getClazz() {
		return clazz;
	}
	
	/**
	 * Returns mapping information about specified property of the class 
	 * @param name
	 * 		name of the property
	 * @return
	 */
	public PropertyInfo getPropertyInfo(String name) {
		return properties.get(name);
	}
	
	/**
	 * Returns a list of class properties
	 * @return
	 */
	public Collection<PropertyInfo> getProperties() {
		return properties.values();
	}

		
}
