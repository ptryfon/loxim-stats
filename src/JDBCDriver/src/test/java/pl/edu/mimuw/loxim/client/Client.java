package pl.edu.mimuw.loxim.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.sql.Connection;
import java.sql.Driver;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Properties;

import pl.edu.mimuw.loxim.jdbc.LoXiMDriverImpl;

public class Client {

	public static void main(String[] args) throws SQLException, IOException {
		if (args.length != 3) {
			System.out.println("Wywo�anie:\njava " + Client.class.getName() + " <JDBC db url> <user> <password>");
		}
		Connection con = null;
		try {
			String url = args[0];
			Driver driver = new LoXiMDriverImpl();
			Properties info = new Properties();
			info.setProperty("user", args[1]);
			info.setProperty("password", args[2]);
			con = driver.connect(url, info);
			Client cli = new Client();
			cli.execute(con, new InputStreamReader(System.in));
		} finally {
			if (con != null) {
				con.close();
			}
		}
	}

	public Client() {

	}
	
	public void execute(Connection con, Reader reader) throws IOException, SQLException {
		BufferedReader inReader = new BufferedReader(reader);
		for (String sbql = inReader.readLine(); (sbql != null) && (!sbql.equals("quit")); sbql = inReader.readLine()) {
			if (!sbql.trim().isEmpty()) { 
				Statement stmt = con.createStatement();
				ResultSet result = stmt.executeQuery(sbql);
				System.out.println("ResultSet:");
				System.out.println(result);
			}
		}
	}
	
}
