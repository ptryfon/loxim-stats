package pl.edu.mimuw.loxim.jdbc;

import java.util.Properties;

class ConnectionInfo {

	private String host;
	private int port;
	private String db;
	private Properties info;
	
	public ConnectionInfo(String host, String db) {
		this(host, db, null);
	}

	public ConnectionInfo(String host, int port, String db) {
		this(host, port, db, null);
	}
	
	public ConnectionInfo(String host, String db, Properties info) {
		this(host, LoXiMProperties.defaultPort, db, info);
	}
	
	public ConnectionInfo(String host, int port, String db, Properties info) {
		super();
		this.host = host;
		this.port = port;
		this.db = db;
		this.info = info;
	}

	public String getHost() {
		return host;
	}
	
	public int getPort() {
		return port;
	}

	public String getDb() {
		return db;
	}

	public String getProperty(String key) {
		return info.getProperty(key);
	}
	
	public String getProperty(String key, String defaultValue) {
		return info.getProperty(key, defaultValue);
	}
		
	public void setProperty(String key, String value) {
		info.setProperty(key, value);
	}
	
	public void setInfo(Properties info) {
		this.info = info;
	}
	
	public void setTimeout(int timeout) {
		setProperty("timeout", String.valueOf(timeout));
	}
	
	public String getUser() {
		return getProperty("user");
	}
	
	public String getPassword() {
		return getProperty("password");
	}
	
	public int getTimeout() {
		return Integer.parseInt(getProperty("timeout", "0"));
	}
	
	@Override
	public String toString() {
		return host + ":" + port + DatabaseURL.CONNECTION_SEPARATOR + db + " " + info;
	}
}
