package pl.tzr.transparent.structure.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Annotation used to mark setter of bean property that should be mapped
 * to a composite value in the database. <br/>
 * Use this annotation when you want to have the property stored as 
 * composite node.
 * @author Tomasz Rosiek
 *
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Component {
    
    /**
     * Defines what to do with the node representing old value of the property
     * when the property value is changed. Default action is delete.
     */
    OnRemoveAction onRemove() default OnRemoveAction.DELETE;
    
    /**
     * Defines what to do when we try to bind an existing database object
     * as a child of some component object
     */
    OnBindAction onCreate() default OnBindAction.COPY;
}
