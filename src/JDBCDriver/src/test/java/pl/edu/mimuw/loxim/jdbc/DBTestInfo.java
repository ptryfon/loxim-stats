package pl.edu.mimuw.loxim.jdbc;

final class DBTestInfo {
	private static final String host = "localhost";
	public static final String DB_URL = "jdbc:loxim:" + host + "/db";
	public static final String DB_URL_WITH_PORT = "jdbc:loxim:" + host + ":2000/db";
	public static final String DB_USER = "root";
	public static final String DB_PASSWORD = "";
}
