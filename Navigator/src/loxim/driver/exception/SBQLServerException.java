package loxim.driver.exception;

/**
 * Wyjatek zwracany w przypadku otrzymania komunikatu o bledzie z serwera bazy danych LoXiM
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class SBQLServerException extends SBQLException {
	
	private static final long serialVersionUID = 1L;
	long errorCode;
	
	/**
	 * Zwraca numer modulu w ktorym wystapil blad (patrz zrodla bazy danych)
	 * @return kod bledu
	 */
	public String getModuleCode() {
		return Integer.toHexString((int)(errorCode >> 20));
	}
	
	/**
	 * Zwraca kod bledu (patrz zrodla bazy danych)
	 * @return kod bledu
	 */
	public String getDetailCode() {
		return Integer.toHexString((int)errorCode & 0xFFFFF);
	}
	
	public SBQLServerException(long _errorCode) {		
		super();
		errorCode = _errorCode;
	}	
	
	public long getErrorCode() {
		return errorCode;
	}

	@Override
	public String getMessage() {
		return "Server execution error (module:" + getModuleCode() + " errno:" + getDetailCode() + ")";
	}	
}
