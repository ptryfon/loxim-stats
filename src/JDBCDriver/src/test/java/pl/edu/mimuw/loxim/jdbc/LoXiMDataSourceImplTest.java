package pl.edu.mimuw.loxim.jdbc;

import static org.junit.Assert.assertTrue;

import java.sql.Connection;
import java.sql.SQLInvalidAuthorizationSpecException;

import org.junit.Ignore;
import org.junit.Test;

@Ignore
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
		con.close();
	}
	
	@Test
	public void testGetConnectionWithUsernameAndPassword() throws Exception {
		ds = new LoXiMDataSourceImpl(DBTestInfo.DB_URL);
		Connection con = ds.getConnection(DBTestInfo.DB_USER, DBTestInfo.DB_PASSWORD);
		assertTrue(con.isValid(0));
		con.close();
	}

	@Test(expected=SQLInvalidAuthorizationSpecException.class)
	public void testGetConnectionWithWrongAuth1() throws Exception {
		ds = new LoXiMDataSourceImpl(DBTestInfo.DB_URL, null, null);
		ds.getConnection();
	}
	
	@Test(expected=SQLInvalidAuthorizationSpecException.class)
	public void testGetConnectionWithWrongAuth2() throws Exception {
		ds = new LoXiMDataSourceImpl(DBTestInfo.DB_URL);
		ds.getConnection(null, null);
	}
}
