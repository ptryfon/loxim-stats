package pl.edu.mimuw.loxim.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.sql.Connection;
import java.sql.Driver;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Properties;

import pl.edu.mimuw.loxim.jdbc.LoXiMDriverImpl;

public class Client {

	public static void main(String[] args) throws SQLException, IOException {
		String url = "jdbc:loxim:localhost/db"; // args[0];
		Driver driver = new LoXiMDriverImpl();
		Properties info = new Properties();
		info.setProperty("user", "root");
		info.setProperty("password", "");
		Connection con = driver.connect(url, info);
		BufferedReader inReader = new BufferedReader(new InputStreamReader(System.in));
		for (String sbql = inReader.readLine(); !sbql.equals("quit"); sbql = inReader.readLine()) {
			Statement stmt = con.createStatement();
			ResultSet result = stmt.executeQuery(sbql);
			System.out.println("ResultSet:");
			System.out.println(result);
			con.commit();
		}
		con.close();
	}

}
