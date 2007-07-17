package pl.tzr.driver.loxim;

import java.io.IOException;
import java.net.Socket;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.exception.SBQLIOException;

/**
 * Simple implementation of semistructural datasource providing connection
 * to the LoXiM database.
 * This datasource creates connections on demand and disconnects when released.
 * @author Tomasz Rosiek
 *
 */
public class LoximDatasourceImpl implements LoximDatasource {

	private String host;
	
	private int port;
	
	private String login;
	
	private String password;	

	public SimpleConnection getConnection() throws SBQLException {
		try {
			Socket sock = new Socket(host, port);
			TcpConnection connection = new TcpConnection(sock);
			return new SimpleConnectionImpl(connection, login, password);
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
		
	}
	
	public void release(SimpleConnection connection) throws SBQLException {
		connection.close();		
	}	

	public String getHost() {
		return host;
	}

	/**
	 * Sets host name or ip address of the database server
	 * @param host hostname or ip addres
	 */
	public void setHost(String host) {
		this.host = host;
	}

	public String getLogin() {
		return login;
	}

	/**
	 * Sets user's login to be used during the database connection
	 * @param login user's login
	 */
	public void setLogin(String login) {
		this.login = login;
	}

	public String getPassword() {
		return password;
	}

	/**
	 * Sets user's password to be used during the database connection
	 * @param login user's passoword
	 */	
	public void setPassword(String password) {
		this.password = password;
	}

	public int getPort() {
		return port;
	}

	/**
	 * Sets database servers TCP port
	 * @param login user's login
	 */	
	public void setPort(int port) {
		this.port = port;
	}

}
