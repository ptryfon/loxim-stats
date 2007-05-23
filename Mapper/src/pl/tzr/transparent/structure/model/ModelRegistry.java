package pl.tzr.transparent.structure.model;

import java.util.HashMap;
import java.util.Map;


/**
 * Represents the data model and mapping definition. Contains the list of
 * all classes wich could be mapped to the LoXiM database with the
 * way of mapping
 * @author Tomasz Rosiek
 *
 */
public class ModelRegistry {

	private Map<Class, ClassInfo>  registry = new HashMap<Class, ClassInfo>();
	private Map<String, ClassInfo>  registryByName = new HashMap<String, ClassInfo>();
	
	public void registerClass(ClassInfo classInfo) {
		
		registry.put(classInfo.getClazz(), classInfo);
		registryByName.put(classInfo.getEntityName(), classInfo);
		
	}
	
	public ClassInfo getClassInfo(Class clazz) {
		//FIXME what about inheritance ?
		return registry.get(clazz);
	}
	
	public ClassInfo getClassInfo(String entityName) {
		return registryByName.get(entityName);
	}
	
}
