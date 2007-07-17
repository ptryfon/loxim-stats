package pl.tzr.transparent.structure.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Annotation used to mark the class that objects of this class can be
 * persisted in the database 
 * @author Tomasz Rosiek
 *
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface Node {
    /**
     * Defines the name of database nodes representing the objects of this
     * class in the database. Default value is the name of the class
     * @return
     */
    String nodeName() default "";
}
