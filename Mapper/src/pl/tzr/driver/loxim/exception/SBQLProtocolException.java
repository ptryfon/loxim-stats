package pl.tzr.driver.loxim.exception;

/**
 * Wyjatek zglaszany w przypadku stwierdzenia bledow protokolu komunikacyjnego LoXiM. W wiekszosci przypadkow
 * bedzie on oznaczal ze uzywamy nowej wersji serwera bazodanowego, w ktorej wprowadzono zmiany w protokole.
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class SBQLProtocolException extends SBQLException {
	
	private static final long serialVersionUID = 8910104042933645958L;

	public SBQLProtocolException(String message) {
		super(message);
	}
	
}
