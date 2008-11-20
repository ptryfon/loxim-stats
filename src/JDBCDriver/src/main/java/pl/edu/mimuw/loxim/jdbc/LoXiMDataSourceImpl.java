package pl.edu.mimuw.loxim.jdbc;

import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.SQLException;
import java.util.Properties;

public class LoXiMDataSourceImpl implements LoXiMDataSource {

	private String databaseUrl;
	private String user;
	private String password;
	private int loginTimeout; 
	
	private transient PrintWriter logWriter;
	
	public LoXiMDataSourceImpl() {
		
	}

	public LoXiMDataSourceImpl(String user, String password) {
		this(null, user, password);
	}

	public LoXiMDataSourceImpl(String databaseUrl, String user, String password) {
		this.databaseUrl = databaseUrl;
		this.user = user;
		this.password = password;
	}

	public LoXiMDataSourceImpl(String databaseUrl) {
		this(databaseUrl, null, null);
	}



	@Override
	public Connection getConnection() throws SQLException {
		return getConnection(user, password);
	}

	@Override
	public LoXiMConnection getConnection(String username, String password) throws SQLException {
        Properties info = new Properties();

        if (username != null) {
            info.setProperty("user", username);
        }

        if (password != null) {
            info.setProperty("password", password);
        }
		
        info.setProperty("timeout", String.valueOf(loginTimeout));
        
		return LoXiMDriverImpl.getConnection(databaseUrl, info);
	}

	@Override
	public PrintWriter getLogWriter() throws SQLException {
		return logWriter;
	}

	@Override
	public int getLoginTimeout() throws SQLException {
		return loginTimeout / 1000;
	}

	@Override
	public void setLogWriter(PrintWriter out) throws SQLException {
		logWriter = out;
	}

	@Override
	public void setLoginTimeout(int seconds) throws SQLException {
		loginTimeout = seconds * 1000;
	}

	@Override
	public boolean isWrapperFor(Class<?> iface) throws SQLException {
		return LoXiMDataSource.class.equals(iface);
	}

	@Override
	public <T> T unwrap(Class<T> iface) throws SQLException {
		if (isWrapperFor(iface)) {
			return (T) this;
		}
		throw new SQLException("Not a wrapper for " + iface);
	}

	public String getDatabaseUrl() {
		return databaseUrl;
	}

	public void setDatabaseUrl(String databaseUrl) {
		this.databaseUrl = databaseUrl;
	}

	public String getUser() {
		return user;
	}

	public void setUser(String user) {
		this.user = user;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}
}
