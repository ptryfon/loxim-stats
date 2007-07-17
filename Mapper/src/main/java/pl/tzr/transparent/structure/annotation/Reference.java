package pl.tzr.transparent.structure.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Annotation used to mark setter of bean attribute that should be mapped
 * as a reference. <br/>
 * Use this annotation when you want to have the attribute stored as 
 * reference value to node representing attribute value. <br/>
 * This way of mapping attributes is default for non-primitive attributes
 * @author Tomasz Rosiek
 *
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Reference {
    /* ... */
}
