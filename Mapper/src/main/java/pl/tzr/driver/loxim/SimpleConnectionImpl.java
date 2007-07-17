package pl.tzr.driver.loxim;

import java.util.HashMap;
import java.util.Map;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.exception.SBQLProtocolException;
import pl.tzr.driver.loxim.result.Result;
import pl.tzr.driver.loxim.result.ResultBool;

/**
 * Wrapper for the Connection introducing some useful higher level operations
 * like authentication and transaction support based on the SBQL language.
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class SimpleConnectionImpl implements Connection, SimpleConnection {
	
	private final Connection target;
	
	/**
	 * Opens authenticated connection to the SBQL based database and logs in
	 * @param connection created connection
	 * @param login user's login
	 * @param password user's password
	 * @throws SBQLException
	 */
	public SimpleConnectionImpl(
			final Connection connection, 
			final String login, 
			final String password) 
		throws SBQLException{
		
		target = connection;
		login(login, password);
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
	


	public Result executeNamed(String query, Map<String, Result> params) throws SBQLException {
		long statementId = parse(query);
		return execute(statementId, params);
	}
	

	public Result executeParam(String query, Result... params) throws SBQLException {
		
		Map<String, Result> paramMap = new HashMap<String, Result>();
		
		int i = 0;
		
		StringBuffer buffer = new StringBuffer();
		
		boolean inString = false;
		
		for (char c : query.toCharArray()) {
			
			switch (c) {
			case '\"': 
                buffer.append(c);
				inString = !inString;
				break;
			case '?' :
				if (!inString) {
					if (i >= params.length) throw 
						new IllegalArgumentException("Invalid argument count");
					String paramName = "$arg" + i;
					paramMap.put(paramName, params[i]);					
					buffer.append(paramName);
					i++;
				} else buffer.append('?');
				break;
			default:
				buffer.append(c);

			}
		}
		
		if (i < params.length) 
			throw new IllegalArgumentException("Invalid argument count");
		
		String newQuery = buffer.toString();
		
		long statementId = parse(newQuery);				
		
		return execute(statementId, paramMap);
	}
	
	/**
	 * Starts a transaction
	 * @throws SBQLException
	 */
	public void beginTransaction() throws SBQLException {
		execute("begin");
	}
	
	/**
	 * Commits the transaction
	 * @throws SBQLException
	 */
	public void commitTransation() throws SBQLException {
		execute("end");
	}
	
	/**
	 * Rollbacks the transaction
	 * @throws SBQLException
	 */
	public void rollbackTransaction() throws SBQLException {
		execute("abort");
	}
	
	/**
	 * Tries to login into the database
	 * @param login username
	 * @param password password
	 * @return true, if logging in succeeded, false otherwise
	 * @throws SBQLException
	 */
	protected boolean login(String login, String password) throws SBQLException {
		execute("begin");
		Result res;
		if ("".equals(password)) {
			res = execute("validate " + login + " not_a_password");
		} else {
			res = execute("validate " + login + " " + password);
		}
		execute("end");
		if (!(res instanceof ResultBool)) 
			throw new SBQLProtocolException("Unknown result");
		
		return ((ResultBool)res).getValue();		
	}	
	
}

