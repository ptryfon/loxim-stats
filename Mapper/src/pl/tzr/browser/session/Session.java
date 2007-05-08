package pl.tzr.browser.session;

import java.util.Set;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;


/**
 * @author Tomasz Rosiek
 *
 */
public interface Session {

	/**
	 * Wykonuje zapytanie SBQL
	 * @param query zapytanie SBQL
	 * @return zbiór obiektów stanowiących wynik zapytania
	 */
	Set<Node> find(String query) throws SBQLException;
	
	/**
	 * Tworzy nowy prosty obiekt bazodanowy
	 * @param name nazwa obiektu
	 * @param value wartość
	 * @return reprezentant nowoutworzonego obiektu
	 */
	Node createString(String name, String value) throws SBQLException;
	
	Node createInt(String name, int value) throws SBQLException;
	
	Node createBoolean(String name, boolean value) throws SBQLException;
	
	/**
	 * Tworzy nowy złożony obiekt bazodanowy - początkowo bez zawartości
	 * @param name
	 * @return
	 */
	//ObjectProxy createComplex(String name);
	
}
