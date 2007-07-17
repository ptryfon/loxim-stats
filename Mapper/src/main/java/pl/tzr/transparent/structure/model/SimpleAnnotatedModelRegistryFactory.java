package pl.tzr.transparent.structure.model;

import java.lang.annotation.Annotation;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import pl.tzr.transparent.proxy.handler.ComponentPropertyAccessor;
import pl.tzr.transparent.proxy.handler.ComponentSetPropertyAccessor;
import pl.tzr.transparent.proxy.handler.PrimitiveSetPropertyAccessor;
import pl.tzr.transparent.proxy.handler.PropertyAccessor;
import pl.tzr.transparent.proxy.handler.ReferencePropertyAccessor;
import pl.tzr.transparent.proxy.handler.ReferenceSetPropertyAccessor;
import pl.tzr.transparent.proxy.handler.registry.AccessorRegistry;
import pl.tzr.transparent.structure.annotation.Component;
import pl.tzr.transparent.structure.annotation.ComponentSet;
import pl.tzr.transparent.structure.annotation.Node;
import pl.tzr.transparent.structure.annotation.Persistent;
import pl.tzr.transparent.structure.annotation.PrimitiveSet;
import pl.tzr.transparent.structure.annotation.Reference;
import pl.tzr.transparent.structure.annotation.ReferenceSet;

/**
 * Data model factory componet wich creates data model and mapping definition
 * based on annotated Java classes of the model.
 * 
 * @author Tomasz Rosiek
 * 
 */
public class SimpleAnnotatedModelRegistryFactory implements
        ModelRegistryFactory {

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

    @SuppressWarnings("unchecked")
    private ClassInfo createClassInfo(Class clazz, ModelRegistry modelRegistry) {
        
        String nodeName = clazz.getSimpleName();
        
        Node nodeAnnotation = (Node)clazz.getAnnotation(Node.class);
        if (nodeAnnotation != null) {
            if (!"".equals(nodeAnnotation.nodeName())) {
                nodeName = nodeAnnotation.nodeName();
            }
        }

        Map<String, PropertyInfo> properties = new HashMap<String, PropertyInfo>();

        for (Method method : clazz.getDeclaredMethods()) {

            if (method.getName().startsWith("get")) {

                String propertyName = method.getName().substring(3);

                propertyName = Character.toLowerCase(propertyName.charAt(0))
                        + propertyName.substring(1);

                PropertyInfo propertyInfo = createPropertyInfo(propertyName,
                        method.getReturnType(), method.getAnnotations(),
                        modelRegistry);

                properties.put(propertyName, propertyInfo);

            }

        }

        ClassInfo classInfo = new ClassInfo(nodeName, clazz,
                properties);

        return classInfo;

    }

    private Map<Class<? extends Annotation>, Annotation> toAnnotationMap(
            Annotation[] annotations) {

        Map<Class<? extends Annotation>, Annotation> annotationMap = 
            new HashMap<Class<? extends Annotation>, Annotation>();

        for (Annotation annotation : annotations) {

            annotationMap.put(annotation.annotationType(), annotation);

        }

        return annotationMap;

    }

    private PropertyInfo createPropertyInfo(String propertyName,
            Class returnType, Annotation[] annotations,
            ModelRegistry modelRegistry) {

        Map<Class<? extends Annotation>, Annotation> annotationMap = 
            toAnnotationMap(annotations);
        
        String nodeName = propertyName;
        
        if (annotationMap.containsKey(Persistent.class)) {
            
            Persistent persistentAnnotation = (Persistent)annotationMap.get(Persistent.class);
            if (!"".equals(persistentAnnotation.nodeName())) {
                nodeName = persistentAnnotation.nodeName();
            }
            
        }

        PropertyInfo propertyInfo = null;

        if (annotationMap
                .containsKey(pl.tzr.transparent.structure.annotation.ComponentSet.class)) {

            ComponentSet persistentSetAnnotation = (ComponentSet) annotationMap
                    .get(ComponentSet.class);

            if (!(Set.class.isAssignableFrom(returnType)))
                throw new IllegalStateException(
                        "Niewlasciwy typ danych dla obiektu klasy ComponentSet");

            propertyInfo = new CollectionPropertyInfo(propertyName, 
                    nodeName, Set.class,
                    persistentSetAnnotation.itemType(),
                    new ComponentSetPropertyAccessor());

        } else if (annotationMap.containsKey(ReferenceSet.class)) {

            ReferenceSet persistentSetAnnotation = (ReferenceSet) annotationMap
            .get(ReferenceSet.class);

            if (!(Set.class.isAssignableFrom(returnType)))
                throw new IllegalStateException(
                        "Niewlasciwy typ danych dla obiektu klasy ReferenceSet");
        
            propertyInfo = new CollectionPropertyInfo(propertyName, 
                    nodeName, Set.class,
                    persistentSetAnnotation.itemType(),
                    new ReferenceSetPropertyAccessor());
            
        } else if (annotationMap.containsKey(PrimitiveSet.class)) {
            
            PrimitiveSet persistentSetAnnotation = (PrimitiveSet) annotationMap
            .get(PrimitiveSet.class);

            if (!(Set.class.isAssignableFrom(returnType)))
                throw new IllegalStateException(
                        "Niewlasciwy typ danych dla obiektu klasy PrimitiveSet");
        
            propertyInfo = new CollectionPropertyInfo(propertyName, 
                    nodeName, Set.class,
                    persistentSetAnnotation.itemType(),
                    new PrimitiveSetPropertyAccessor());
            
            
        } else if (annotationMap.containsKey(Reference.class)) {

            propertyInfo = new PropertyInfo(propertyName, nodeName, returnType,
                    new ReferencePropertyAccessor());

        } else if (annotationMap.containsKey(Component.class)) {
            
            Component componentAnnotation = (Component)
                annotationMap.get(Component.class);
            
            propertyInfo = new PropertyInfo(propertyName, nodeName, returnType,
                    new ComponentPropertyAccessor(componentAnnotation.onRemove()));
            
        } else if (accessorRegistry.isHandlerAvailable(returnType)) {

            PropertyAccessor handler = accessorRegistry.getHanlder(returnType);

            propertyInfo = new PropertyInfo(propertyName, nodeName, 
                    returnType, handler);

        } else {
            
            /* Default mapping is the reference */
            
            /* TODO - check whether the target type is properly mapped */
            
            propertyInfo = new PropertyInfo(propertyName, nodeName, returnType,
                    new ReferencePropertyAccessor());
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
