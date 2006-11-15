package loxim.driver;

import java.io.IOException;
import java.net.Socket;

import loxim.driver.exception.SBQLException;
import loxim.driver.exception.SBQLIOException;


/**
 * Fabryka sluzy do otwierania nowych polaczen z baza danych LoXiM opartych o protokol TCP/IP
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class TcpConnectionFactory {
	/**
	 * Otwiera nowe polaczenie z baza danych
	 * @param host adres serwera bazy danych
	 * @param port port serwera
	 * @return obiekt reprezentujacy polaczenie z baza
	 * @throws SBQLException
	 */
	public static Connection getConnection(String host, int port) throws SBQLException {
		try {
			Socket sock = new Socket(host, port);
			return new TcpConnection(sock);
		} catch (IOException e) {
			throw new SBQLIOException(e);
		}
	}
}
