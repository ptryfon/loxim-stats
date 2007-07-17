package pl.tzr.transparent.structure.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Annotation used to mark setter of bean property that should be mapped
 * as a set of simple values (booleab, integer, string) in the database. <br/>
 * Use this annotation when you want to have the property value stored as 
 * a set of simple nodes. <br/>
 * Can only be applied to properties of Set type.
 * @author Tomasz Rosiek
 *
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface PrimitiveSet {
    /**
     * Defines an item type that can be stored in the set. Can
     * be Boolean, Integer and String. Required.
     */
    Class itemType();
}
