package pl.edu.mimuw.loxim.jdbc;

import static org.junit.Assert.assertTrue;

import java.sql.Connection;

import org.junit.Test;

public class LoXiMDataSourceImplTest {

	private LoXiMDataSourceImpl ds;
	
	@Test(expected=IllegalArgumentException.class)
	public void testGetConnectionWithNoData() throws Exception {
		ds = new LoXiMDataSourceImpl();
		ds.getConnection();
	}

	@Test
	public void testGetConnection() throws Exception {
		ds = new LoXiMDataSourceImpl(DBTestInfo.DB_URL, DBTestInfo.DB_USER, DBTestInfo.DB_PASSWORD);
		Connection con = ds.getConnection();
		assertTrue(con.isValid(0));
	}
	
	@Test
	public void testGetConnectionWithUsernameAndPassword() throws Exception {
		ds = new LoXiMDataSourceImpl(DBTestInfo.DB_URL);
		Connection con = ds.getConnection(DBTestInfo.DB_USER, DBTestInfo.DB_PASSWORD);
		assertTrue(con.isValid(0));
	}

}
