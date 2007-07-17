package pl.tzr.exception;

import pl.tzr.driver.loxim.exception.SBQLException;

/**
 * Unchecked wrapper for the LoXiM database/connection exceptions
 * @author Tomasz Rosiek
 *
 */
public class NestedSBQLException extends MappingException {
    
    private static final long serialVersionUID = 1L;    
	
	public NestedSBQLException(SBQLException e) {
		super(e);
	}
	
}
