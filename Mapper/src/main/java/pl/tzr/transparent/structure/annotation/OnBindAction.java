/**
 * 
 */
package pl.tzr.transparent.structure.annotation;

/**
 * Defines what to do when we try to bind an existing database object
 * as a child of some component object
 * @author Tomasz Rosiek
 *
 */
public enum OnBindAction {
    /**
     * Binds instance of the object as a child of the target component object.
     * Not implemented.
     */
    BIND_INSTANCE, 
    /**
     * Creates a copy of the object and binds the copy as a child of the
     * target component object 
     */
    COPY, 
    /**
     * Prohibits rebinding existing objects 
     */
    DENY
}
