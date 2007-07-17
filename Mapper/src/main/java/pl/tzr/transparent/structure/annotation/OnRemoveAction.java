package pl.tzr.transparent.structure.annotation;

/**
 * Defines what to do with the node representing deleted value
 */  
public enum OnRemoveAction {
    /**
     * Move the node from actual place to the root. 
     */
    MOVE_TO_ROOT,
    
    /**
     * Delete the node from the storage.
     */
    DELETE
}
