package pl.tzr.exception;

import pl.tzr.driver.loxim.exception.SBQLException;

/**
 * Wrapper for the LoXiM database/connection exceptions
 * @author Tomasz Rosiek
 *
 */
public class NestedSBQLException extends MappingException {
	
	private SBQLException e;

	public NestedSBQLException(SBQLException e) {
		this.e = e;
	}
	
}
