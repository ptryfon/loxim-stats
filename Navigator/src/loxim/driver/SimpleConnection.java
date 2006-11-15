package loxim.driver;

import java.util.HashMap;
import java.util.Map;

import loxim.driver.exception.SBQLException;
import loxim.driver.exception.SBQLProtocolException;
import loxim.driver.result.Result;
import loxim.driver.result.ResultBool;
import loxim.driver.result.ResultReference;


/**
 * Wrapper na obiekty reprezentujące połączenie z baza danych LoXiM, pozwalajacy na wykonywanie bardziej wysokopoziomowych operacji
 * jak logowanie czy rozpoczynanie i konczenie transakcji 
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class SimpleConnection implements Connection {
	Connection target;
	
	public SimpleConnection(Connection connection) {
		target = connection;
	}

	public void close() throws SBQLException {
		target.close();
	}

	public Result execute(String query) throws SBQLException {
		return target.execute(query);
	}

	public Result execute(long statementId, Map<String, Result> params)
			throws SBQLException {
		return target.execute(statementId, params);
	}

	public long parse(String query) throws SBQLException {
		return target.parse(query);
	}
	

	/**
	 * Wykonuje zapytanie sparametryzowane
	 * @param query tresc zapytania
	 * @param params parametry zapytania
	 * @return
	 * @throws SBQLException
	 */
	public Result execute(String query, Map<String, Result> params) throws SBQLException {
		long statementId = parse(query);
		return execute(statementId, params);
	}
	
	/**
	 * Wykonuje zapytanie sparametryzowane - parametry z tablicy podstawia w kolejnosci wystepowania.
	 * @param query
	 * @param params
	 * @return
	 * @throws SBQLException
	 */
	public Result execute(String query, Result[] params) throws SBQLException {
		throw new RuntimeException("Nie zaimplementowane");
	}
	
	/**
	 * Probuje sie zalogowac do bazy danych
	 * @param login nazwa uzytkownika
	 * @param password haslo
	 * @return true, jesli udalo sie zalogowac, false w przeciwnym przypadku
	 * @throws SBQLException
	 */
	public boolean login(String login, String password) throws SBQLException {
		execute("begin");
		Result res;
		if ("".equals(password)) {
			res = execute("validate " + login + " not_a_password");
		} else {
			res = execute("validate " + login + " " + password);
		}
		execute("end");
		if (!(res instanceof ResultBool)) throw new SBQLProtocolException("Unknown result");		
		return ((ResultBool)res).getValue();		
	}
	
	/**
	 * Rozpoczyna transakcje
	 * @throws SBQLException
	 */
	public void beginTransaction() throws SBQLException {
		execute("begin");
	}
	
	/**
	 * Zatwierdza transakcje
	 * @throws SBQLException
	 */
	public void commitTransation() throws SBQLException {
		execute("end");
	}
	
	/**
	 * Przerywa transakcje
	 * @throws SBQLException
	 */
	public void rollbackTransaction() throws SBQLException {
		execute("abort");
	}
	
	/**
	 * Dokonuje dereferencji obiektu
	 * @param ref referencja
	 * @return obiekt okreslony przez referencje
	 */
	public Result deref(String ref) throws SBQLException {
		Map<String, Result> m = new HashMap<String, Result>();
		m.put("$ref", new ResultReference(ref));
		return execute("deref($ref)", m);		
	}
}

