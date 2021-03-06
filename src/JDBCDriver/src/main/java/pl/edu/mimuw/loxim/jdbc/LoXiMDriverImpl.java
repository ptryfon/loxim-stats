package pl.edu.mimuw.loxim.jdbc;

import java.io.IOException;
import java.sql.DriverManager;
import java.sql.DriverPropertyInfo;
import java.sql.SQLException;
import java.util.Properties;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import pl.edu.mimuw.loxim.protogen.lang.java.template.auth.AuthException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;

public class LoXiMDriverImpl implements LoXiMDriver {

	private static final Log log = LogFactory.getLog(LoXiMDriverImpl.class);

	static {
		try {
			DriverManager.registerDriver(new LoXiMDriverImpl());
		} catch (SQLException e) {
			if (log.isFatalEnabled()) {
				log.fatal("Could not register LoXiM driver in DriverManager", e);
			}
		}
	}

	public LoXiMDriverImpl() {

	}

	@Override
	public boolean acceptsURL(String url) throws SQLException {
		if (url == null) {
			return false;
		}
		return url.matches(DatabaseURL.PROTOCOL_REGEX);
	}

	@Override
	public LoXiMConnection connect(String jdbcURL, Properties info) throws SQLException {
		if (acceptsURL(jdbcURL)) {
			return getConnection(jdbcURL, info);
		}
		if (log.isErrorEnabled()) {
			log.error("The URL: " + jdbcURL + " is not accepted");
		}
		return null;
	}

	static LoXiMConnection getConnection(String jdbcURL, Properties info) throws SQLException {
		try {
			if (jdbcURL == null) {
				throw new IllegalArgumentException("JDBC URL is null");
			}
			ConnectionInfo conInfo = DatabaseURL.parseURL(jdbcURL);
			conInfo.setInfo(info);
			return new LoXiMConnectionImpl(conInfo);
		} catch (IOException e) {
			throw new SQLException(e);
		} catch (ProtocolException e) {
			throw new SQLException(e);
		} catch (AuthException e) {
			throw new SQLException(e);
		}
	}

	@Override
	public int getMajorVersion() {
		return LoXiMProperties.LoXiMDriverMajor;
	}

	@Override
	public int getMinorVersion() {
		return LoXiMProperties.LoXiMDriverMinor;
	}

	@Override
	public DriverPropertyInfo[] getPropertyInfo(String url, Properties info) throws SQLException {
		DriverPropertyInfo[] pinfo = new DriverPropertyInfo[2];
		DriverPropertyInfo p;

		p = new DriverPropertyInfo("user", null);
		p.value = info.getProperty("user");
		p.required = true;
		pinfo[0] = p;

		p = new DriverPropertyInfo("password", null);
		p.value = info.getProperty("password");
		p.required = true;
		pinfo[1] = p;

		return pinfo;
	}

	/**
	 * Returns false.
	 * 
	 * @see java.sql.Driver#jdbcCompliant()
	 */
	@Override
	public boolean jdbcCompliant() {
		return false;
	}

}
