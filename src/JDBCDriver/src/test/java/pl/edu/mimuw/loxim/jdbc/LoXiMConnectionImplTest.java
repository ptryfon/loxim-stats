package pl.edu.mimuw.loxim.jdbc;

import static org.junit.Assert.*;

import java.sql.SQLException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

public class LoXiMConnectionImplTest {

	private static final Log log = LogFactory.getLog(LoXiMConnectionImplTest.class);
	
	private static LoXiMConnectionImpl con;
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		con = (LoXiMConnectionImpl) new LoXiMDataSourceImpl(DBTestInfo.DB_URL, DBTestInfo.DB_USER, DBTestInfo.DB_PASSWORD).getConnection();
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		con.close();
	}
	
	@Test
	public void testSimpleCommit() throws SQLException {
		ExecutionResult res = con.execute(con.createStatement(), "123");
		log.info("Result: " + res.asLoXiMResultSet());
		con.commit();
	}

	@Test
	public void testSimpleRollback() throws SQLException {
		con.execute(con.createStatement(), "1");
		con.rollback();
	}
}
