package pl.tzr.driver.loxim.exception;

/**
 * Abstrakcyjna klasa reprezentująca wyjątek interfejsu do bazy danych LoXiM
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public abstract class SBQLException extends Exception {
	public SBQLException() {
		super();
	}

	protected SBQLException(String message) {
		super(message);
	}
}
