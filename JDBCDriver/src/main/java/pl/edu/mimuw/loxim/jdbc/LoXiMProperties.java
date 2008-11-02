package pl.edu.mimuw.loxim.jdbc;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

class LoXiMProperties {

	private static final String propertiesFile = "/META-INF/loxim.properties";
	private static final Properties prop;
	private static final Log log = LogFactory.getLog(LoXiMProperties.class);
	
	public static final String LoXiMDriverName = "LoXiM JDBC driver";
	public static final int LoXiMDriverMajor = 1;
	public static final int LoXiMDriverMinor = 0;
	public static final int JDBCVerMajor = 4;
	public static final int JDBCVerMinor = 0;
	
	public static final int defaultPort = 2000;
	
	static {
		try {
			prop = loadProperties();
		} catch (IOException e) {
			if (log.isErrorEnabled()) {
				log.error("Could not load LoXiM properties file (" + propertiesFile + ")", e);
			}
			throw new LoXiMRuntimeException(e);
		}
	}
	
	private static Properties loadProperties() throws FileNotFoundException, IOException {
		Properties prop = new Properties();
		InputStream propertiesStream = LoXiMProperties.class.getResourceAsStream(propertiesFile);
		if (propertiesStream != null) {
			prop.load(propertiesStream);
		} else {
			throw new FileNotFoundException("Could not get file \"" + propertiesFile + "\"");
		}
		return null;
	}
	
	public static String getProperty(String key) {
		return prop.getProperty(key);
	}
	
	public static String getProperty(String key, String defaultValue) {
		return prop.getProperty(key, defaultValue);
	}
}
