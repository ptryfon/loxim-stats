package pl.tzr.transparent.structure.model;

import java.util.Collection;
import java.util.Map;


/**
 * Reprezentuje informacje o sposobie reprezentacji obiektów określonej 
 * klasy w bazie danych loxim 
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
	 * Zwraca nazwę pod jaką w bazie danych zostanie zapisany obiekt
	 * tej klasy
	 * @return
	 */
	public String getEntityName() {
		return entityName;
	}

	/**
	 * Zwraca reprezentowaną klasę
	 * @return
	 */
	public Class getClazz() {
		return clazz;
	}
	
	/**
	 * Zwraca informację o atrybucie klasy
	 * @param name
	 * @return
	 */
	public PropertyInfo getPropertyInfo(String name) {
		return properties.get(name);
	}
	
	/**
	 * Zwraca listę atrybutów klasy
	 * @return
	 */
	public Collection<PropertyInfo> getProperties() {
		return properties.values();
	}

		
}
