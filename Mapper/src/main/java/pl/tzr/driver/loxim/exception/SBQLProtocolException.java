package pl.tzr.driver.loxim.exception;

/**
 * Exception thrown in case of LoXiM protocol errors. Usually it means that
 * the driver is not compatibile with current version of the database server. 
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class SBQLProtocolException extends SBQLException {
	
	private static final long serialVersionUID = 8910104042933645958L;

	public SBQLProtocolException(String message) {
		super(message);
	}
	
}
