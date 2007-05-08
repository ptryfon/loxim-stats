package pl.tzr.driver.loxim;

import java.util.Map;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.result.Result;




/**
 * Interfejs opisujący połączenie z bazą LoXiM
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public interface Connection {
	
	/**
	 * Wykonanie prostego zapytania
	 * @param query tresc zapytania
	 * @return wynik zapytania
	 * @throws SBQLException
	 */
	public Result execute(String query) throws SBQLException;
	
	/**
	 * Przygotowanie zapytania sparametryzowanego
	 * @param query tresc zapytania
	 * @return zwraca identyfikator przygotowanego zapytania sparametryzowanego 
	 * @throws SBQLException
	 */
	public long parse(String query) throws SBQLException;
	
	/**
	 * Wykonanie przygotowanego zapytania sparametryzowanego
	 * @param statementId identyfikator przygotowanego zapytania sparametryzowanego
	 * @param params wartosci parametrow
	 * @return wynik zapytania
	 * @throws SBQLException
	 */
	public Result execute(long statementId, Map<String, Result> params) throws SBQLException;
		
	/**
	 * Zamkniecie polaczenia z baza danych
	 * @throws SBQLException
	 */
	public void close() throws SBQLException;
}