package pl.edu.mimuw.loxim.jdbc;

import static org.junit.Assert.*;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

public class LoXiMDriverImplTest {

	private LoXiMDriverImpl driver;
	
	@Before
	public void setUp() {
		driver = new LoXiMDriverImpl();
	}
	
	@Test
	public void testAcceptsURL() throws SQLException {
		String[] good = {DBTestInfo.DB_URL, "jdbc:loxim:students.mimuw.edu.pl:2000/db"};
		String[] bad = {null, "", "aaaaaaaaa", "jdbc", ":", "::", "jdbc:", "jdbc:loxim", "jdbc:loxim:",
				"jdbc:loxim:a", "jdbc:loxim:adadsada:1234:dsadaada", "JDBC:LOXIM:AAA", "jDbC:LoXiM:hElLo", "jdbc:LOXIM:--__--", "jdbc:loxim:a:/ee"};
		assertAcceptsURLs(good, true);
		assertAcceptsURLs(bad, false);
	}

	private void assertAcceptsURLs(String[] urls, boolean acceptResult) throws SQLException {
		for (String s : urls) {
			assertEquals(acceptResult, driver.acceptsURL(s));
		}
	}
	
	@Test
	public void testConnect() throws SQLException {
		Properties info = new Properties();
		info.setProperty("user", "root");
		info.setProperty("password", "");
		Connection con1 = driver.connect(DBTestInfo.DB_URL, info);
		assertTrue(con1.isValid(0));
//		con1.close();
		Connection con2 = driver.connect(DBTestInfo.DB_URL_WITH_PORT, info);
		assertTrue(con2.isValid(0));
//		con2.close();
	}

	@Test
	public void testDriverManagerRegistered() throws Exception {
		Class.forName("pl.edu.mimuw.loxim.jdbc.LoXiMDriverImpl");
		assertEquals(LoXiMDriverImpl.class, DriverManager.getDriver(DBTestInfo.DB_URL).getClass());
	}
	
}
