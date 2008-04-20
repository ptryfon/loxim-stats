package edu.mimuw.loxim.jdbc;

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
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public boolean acceptsURL(String url) throws SQLException {
		// TODO Auto-generated method stub
		return false;
	} 

	@Override
	public Connection connect(String url, Properties info) throws SQLException {
		// TODO Auto-generated method stub
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
		// TODO Auto-generated method stub
		return null;
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
