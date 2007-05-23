package pl.tzr.exception;

import pl.tzr.driver.loxim.exception.SBQLException;

/**
 * Wrapper for the LoXiM database/connection exceptions
 * @author Tomasz Rosiek
 *
 */
public class NestedSBQLException extends MappingException {
	
	public NestedSBQLException(SBQLException e) {
		super(e);
	}
	
}
