package pl.tzr.transparent.structure.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Annotation used to mark setter of bean property that should be mapped
 * as a set of composite values in the database. <br/>
 * Use this annotation when you want to have the property value stored as 
 * a set of composite nodes. <br/>
 * Can only be applied to properties of Set type.
 * @author Tomasz Rosiek
 *
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface ComponentSet {
    /**
     * Defines an item type that can be stored in the set. Required.
     */
	Class itemType();
    
    /**
     * Defines what to do with the node representing deleted item from the set
     * when the item is being deleted.
     * Default action is to delete the node.
     */    
    OnRemoveAction onRemove() default OnRemoveAction.DELETE;
}
