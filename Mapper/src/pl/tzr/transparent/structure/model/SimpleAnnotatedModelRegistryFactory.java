package pl.tzr.transparent.structure.model;

import java.lang.annotation.Annotation;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import pl.tzr.transparent.proxy.handler.ComponentPropertyAccessor;
import pl.tzr.transparent.proxy.handler.PropertyAccessor;
import pl.tzr.transparent.proxy.handler.ReferencePropertyAccessor;
import pl.tzr.transparent.proxy.handler.SetPropertyAccessor;
import pl.tzr.transparent.proxy.handler.registry.AccessorRegistry;
import pl.tzr.transparent.structure.annotation.PersistentSet;
import pl.tzr.transparent.structure.annotation.Reference;

/**
 * Komponent, który tworzy definicję modelu danych w oparciu o klasy
 * Javove tego modelu zawierające odpowiednie adnotacje,
 * informujące o tym jak reprezentować je w bazie danych.
 * @author Tomasz Rosiek
 *
 */
public class SimpleAnnotatedModelRegistryFactory implements ModelRegistryFactory {
	
	private Class[] classes;
	
	private AccessorRegistry accessorRegistry;
	
	public ModelRegistry getModelRegistry() {
		
		ModelRegistry modelRegistry = new ModelRegistry();
		
		for (Class clazz : classes) {
			
			ClassInfo classInfo = createClassInfo(clazz, modelRegistry);
			modelRegistry.registerClass(classInfo);
			
		}
		
		return modelRegistry;
	}

	private ClassInfo createClassInfo(Class clazz, ModelRegistry modelRegistry) {
		
		Map<String, PropertyInfo> properties = 
			new HashMap<String, PropertyInfo>();
		
		for (Method method : clazz.getDeclaredMethods()) {
			
			if (method.getName().startsWith("get")) {
												
				String propertyName = method.getName().substring(3);
				
				propertyName = Character.toLowerCase(propertyName.charAt(0)) + 
				propertyName.substring(1);
				
				PropertyInfo propertyInfo = 
					createPropertyInfo(propertyName, method.getReturnType(), method.getAnnotations(), modelRegistry);
				
				properties.put(propertyName, propertyInfo);
				
			}
			
		}
		
		ClassInfo classInfo = new ClassInfo(clazz.getSimpleName(), clazz, properties);
		
		return classInfo;
		
	}
	
	private Map<Class<? extends Annotation>, Annotation> toAnnotationMap(Annotation[] annotations) {
		
		Map<Class<? extends Annotation>, Annotation> annotationMap = 
			new HashMap<Class<? extends Annotation>, Annotation>();
		
		for (Annotation annotation : annotations) {
			
			annotationMap.put(annotation.annotationType(), annotation);
			
		}
		
		return annotationMap;
		
	}
	
	private PropertyInfo createPropertyInfo(
			String propertyName, Class returnType, Annotation[] annotations, ModelRegistry modelRegistry) {
		
		Map<Class<? extends Annotation>, Annotation> annotationMap = toAnnotationMap(annotations);
		
		PropertyInfo propertyInfo = null;
		
		if (annotationMap.containsKey(pl.tzr.transparent.structure.annotation.PersistentSet.class)) {
			
			PersistentSet persistentSetAnnotation = 
				(PersistentSet)annotationMap.get(PersistentSet.class);
					 			
			if (!(Set.class.isAssignableFrom(returnType))) throw 
				new IllegalStateException("Niewlasciwy typ danych dla obiektu klasy PersistentSet");
			
			propertyInfo = new CollectionPropertyInfo<Set>(propertyName, Set.class,
					persistentSetAnnotation.itemType(), new SetPropertyAccessor());
			
		} else if(annotationMap.containsKey(Reference.class)) {
			
			propertyInfo = new PropertyInfo(propertyName, returnType, 
					new ReferencePropertyAccessor());
			
		}	else if (accessorRegistry.isHandlerAvailable(returnType)) {
			
			PropertyAccessor handler = accessorRegistry.getHanlder(returnType); 
						
			propertyInfo = new PropertyInfo(propertyName, returnType, handler);
			
		} else {
			propertyInfo = new PropertyInfo(propertyName, returnType, 
					new ComponentPropertyAccessor());
		}
				
		return propertyInfo;
	}
	

	public Class[] getClasses() {
		return classes;
	}


	public void setClasses(Class[] classes) {
		this.classes = classes;
	}

	public AccessorRegistry getHandlerRegistry() {
		return accessorRegistry;
	}

	public void setHandlerRegistry(AccessorRegistry accessorRegistry) {
		this.accessorRegistry = accessorRegistry;
	}

}
