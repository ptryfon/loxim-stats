package pl.tzr.driver.loxim;

import java.io.IOException;
import java.net.Socket;

import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.driver.loxim.exception.SBQLIOException;

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

	public String getHost() {
		return host;
	}

	public void setHost(String host) {
		this.host = host;
	}

	public String getLogin() {
		return login;
	}

	public void setLogin(String login) {
		this.login = login;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public int getPort() {
		return port;
	}

	public void setPort(int port) {
		this.port = port;
	}

}
