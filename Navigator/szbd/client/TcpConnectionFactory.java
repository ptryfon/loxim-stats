package szbd.client;

import java.io.IOException;
import java.net.Socket;

public class TcpConnectionFactory {
	public static Connection getConnection(String host, int port) throws IOException {
		Socket sock = new Socket(host, port);
		return new TcpConnection(sock);
	}
}
