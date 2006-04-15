package szbd.console;

import java.net.Socket;

import szbd.client.Connection;
import szbd.client.TcpConnection;
import szbd.client.TcpConnectionFactory;

public class Main {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			ConnectionOptions opts = new ConnectionOptions();
			if (ConnectionDialog.showModal(opts)) {				
				Connection con = TcpConnectionFactory.getConnection(opts.host, opts.port);
				ConsoleFrame inst = new ConsoleFrame(con);
				inst.setLocationRelativeTo(null);
				inst.setVisible(true);				
			}
		} catch (Exception e){
			e.printStackTrace();
		}
	}

}
