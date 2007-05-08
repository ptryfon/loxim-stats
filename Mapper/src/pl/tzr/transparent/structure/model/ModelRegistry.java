package pl.tzr.transparent.structure.model;

import java.util.HashMap;
import java.util.Map;


/**
 * Reprezentuje zbiór informacji o modelu danych. Zawiera wszystkie klasy
 * jakie mogą wystąpić w bazie danych
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
		//FIXME a co z diedziczeniem?
		return registry.get(clazz);
	}
	
	public ClassInfo getClassInfo(String entityName) {
		return registryByName.get(entityName);
	}
	
}
