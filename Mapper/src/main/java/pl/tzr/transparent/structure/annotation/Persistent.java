package pl.tzr.transparent.structure.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Annotation used to that a value of the property should be stored in the 
 * database. Annotation should used to mark setter of the property. 
 * <br/>
 * As default all properties of class marked with Node are persistent
 * 
 * @author Tomasz Rosiek
 *
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Persistent {
    /**
     * Name of the node representing value of the property in the database.
     * Default value is name of the bean property 
     */
    String nodeName() default "";
    
}
