package pl.tzr.transparent.structure.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Annotation used to mark setter of bean property that should be mapped
 * as a collection of references to set values in the database. <br/>
 * Use this annotation when you want to have the property value stored as 
 * a set of reference nodes. <br/>
 * Can only be applied to properties of Set type.
 * @author Tomasz Rosiek
 *
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface ReferenceSet {
    /**
     * Defines an item type that can be stored in the set. Required.
     */
    Class itemType();
}
