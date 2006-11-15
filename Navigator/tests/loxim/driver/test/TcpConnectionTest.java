package loxim.driver.test;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import junit.framework.TestCase;
import loxim.driver.Connection;
import loxim.driver.SimpleConnection;
import loxim.driver.TcpConnectionFactory;
import loxim.driver.exception.SBQLException;
import loxim.driver.exception.SBQLProtocolException;
import loxim.driver.result.Result;
import loxim.driver.result.ResultBag;
import loxim.driver.result.ResultBool;
import loxim.driver.result.ResultInt;
import loxim.driver.result.ResultReference;

public class TcpConnectionTest extends TestCase {
	/**
	 * Testuje proste połączenie się z bazą danych
	 * @throws IOException
	 * @throws SBQLException
	 */
	public void testConnection() throws IOException, SBQLException {
		Connection con = TcpConnectionFactory.getConnection("127.0.0.1", 6543);
		con.close();
	}
	
	private Result doLogin(Connection con, String login, String password) throws SBQLException{
		con.execute("begin");
		Result res;
		if ("".equals(password)) {
			res = con.execute("validate " + login + " not_a_password");
		} else {
			res = con.execute("validate " + login + " " + password);
		}
		con.execute("end");
		if (!(res instanceof ResultBool)) throw new SBQLProtocolException("Unknown result");		
		return res;
	}
	
	/**
	 * Testuje wykonanie prostego zapytania
	 * @throws SBQLException
	 */
	public void testSimpleQuery() throws SBQLException {
		Connection con = TcpConnectionFactory.getConnection("127.0.0.1", 6543);
		doLogin(con, "root", "");
		con.execute("begin");
		Result r = con.execute("5 + 5");
		assert(r instanceof ResultInt);
		con.execute("end");
		assert(((ResultInt)r).getValue() == 10);
		con.close();
	} 

	/**
	 * Testuje wykonanie prostego zapytania sparametryzowanego
	 * @throws IOException
	 * @throws SBQLException
	 */
	public void testParamQuery() throws SBQLException {
		Connection con = TcpConnectionFactory.getConnection("127.0.0.1", 6543);
		doLogin(con, "root", "");
		
		long statementId = con.parse("$a + 5");
		
		System.out.println("StatementId = " + statementId);
		
		/*
		long statementId2 = con.parse("$a + 5");
		
		System.out.println("StatementId = " + statementId2);
		*/
		
		con.execute("begin");
		
		Map<String, Result> params = new HashMap<String, Result>();
		
		params.put("$a", new ResultInt(5));
		
		
		Result res = con.execute(statementId, params);
		System.out.println(res);
		
		con.execute("end");
		
		con.close();
	}
	
	/**
	 * Testuje klase pomocnicza SimpleConnection
	 * @throws SBQLException
	 */
	public void testSimpleConnection() throws SBQLException {
		SimpleConnection con = new SimpleConnection(TcpConnectionFactory.getConnection("127.0.0.1", 6543));
		con.login("root", "");
		con.beginTransaction();
		con.execute("5 + 5");
		con.commitTransation();
		
		con.beginTransaction();
		
		Map<String, Result> params = new HashMap<String, Result>();		
		params.put("$a", new ResultInt(6));		
		System.out.println(con.execute("5 + $a", params));
		con.commitTransation();
		con.close();
	}
	
	public void testDeref() throws SBQLException {
		SimpleConnection con = new SimpleConnection(TcpConnectionFactory.getConnection("127.0.0.1", 6543));
		con.login("root", "");
		
		con.beginTransaction();
		
		Result bag = con.execute("create 5 as test");
		
		
		if (!(bag instanceof ResultBag)) fail();
		
		if (((ResultBag)bag).getItems().size() != 1) fail(); 
		
		Result r = ((ResultBag)bag).getItems().get(0);
		
		if (!(r instanceof ResultReference)) fail();
		
		System.out.println("REF" + ((ResultReference)r).getRef());
		
		Result r2 = con.deref(((ResultReference)r).getRef());
				
		System.out.println(r2);
		
		con.rollbackTransaction();
		con.close();		
	}
}
