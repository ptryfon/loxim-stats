package edu.mimuw.loxim.jdbc;

import java.net.MalformedURLException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.DriverPropertyInfo;
import java.sql.SQLException;
import java.util.Properties;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

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
		return (url != null && url.startsWith(DatabaseURL.PROTOCOL_PREFIX));
	} 

	@Override
	public Connection connect(String jdbcURL, Properties info) throws SQLException {
		if (acceptsURL(jdbcURL)) {
			try {
				ConnectionInfo conInfo = DatabaseURL.parseURL(jdbcURL);
				conInfo.setInfo(info);
				return new LoXiMConnectionImpl(conInfo);
			} catch (MalformedURLException e) {
				throw new SQLException(e);
			}
		}
		if (log.isErrorEnabled()) {
			log.error("The URL: " + jdbcURL + " is not accepted");
		}
		return null;
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
        DriverPropertyInfo[] pinfo   = new DriverPropertyInfo[2];
        DriverPropertyInfo p;
		
        p          = new DriverPropertyInfo("user", null);
        p.value    = info.getProperty("user");
        p.required = true;
        pinfo[0]   = p;
        
        p          = new DriverPropertyInfo("password", null);
        p.value    = info.getProperty("password");
        p.required = true;
        pinfo[1]   = p;
        
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
