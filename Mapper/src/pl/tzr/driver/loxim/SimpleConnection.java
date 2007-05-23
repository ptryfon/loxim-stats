package pl.tzr.driver.loxim;

import java.util.Map;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.result.Result;

public interface SimpleConnection extends Connection {

	/**
	 * Wykonuje zapytanie
	 * @param query tresc zapytania
	 */
	Result execute(String query) throws SBQLException;

	/**
	 * Wykonuje zapytanie sparametryzowane z nazwanymi parametrami
	 * @param query tresc zapytania
	 * @param params parametry zapytania
	 * @throws SBQLException
	 */
	Result executeNamed(String query, Map<String, Result> params)
			throws SBQLException;

	/**
	 * Wykonuje zapytanie sparametryzowane, parametry zapisane jako znaki zapytania
	 * @param query tresc zapytania
	 * @param params parametry zapytania
	 * @throws SBQLException
	 */
	Result executeParam(String query, Result... params) throws SBQLException;

	
	/**
	 * Zamyka polaczenie
	 */
	void close() throws SBQLException;
	

	/**
	 * Rozpoczyna transakcje
	 * @throws SBQLException
	 */
	void beginTransaction() throws SBQLException;

	/**
	 * Zatwierdza transakcje
	 * @throws SBQLException
	 */
	void commitTransation() throws SBQLException;

	/**
	 * Przerywa transakcje
	 * @throws SBQLException
	 */
	void rollbackTransaction() throws SBQLException;

	/**
	 * Dokonuje dereferencji obiektu
	 * @param ref referencja
	 * @return obiekt okreslony przez referencje
	 */
	Result deref(String ref) throws SBQLException;

}