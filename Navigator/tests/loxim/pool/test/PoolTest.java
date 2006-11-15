package loxim.pool.test;


import java.util.LinkedList;
import java.util.List;

import junit.framework.TestCase;
import loxim.driver.SimpleConnection;
import loxim.driver.TcpConnectionFactory;
import loxim.driver.exception.SBQLException;
import loxim.pool.ConnectionService;
import loxim.pool.LoximObject;
import loxim.pool.LoximObjectFactory;
import loxim.pool.LoximReference;
import loxim.pool.ObjectPool;
import loxim.pool.ObjectPoolImpl;

public class PoolTest extends TestCase {
	public void testGetQuery() throws SBQLException {
		ObjectPool pool = new ObjectPoolImpl();
		
		SimpleConnection con = new SimpleConnection(TcpConnectionFactory.getConnection("127.0.0.1", 6543));
		LoximObjectFactory objectFactory = new LoximObjectFactory();
	
		con.login("root", "");
		
		//con.beginTransaction();
		//con.execute("create 5 as kilof");
		//con.commitTransation();
		
		ConnectionService service = new ConnectionService(pool, con, objectFactory);
	
		
		
		con.beginTransaction();
		service.executeQuery("5 as x, 6 as y");
		service.executeQuery("kilof");
		con.commitTransation();
		
		List<LoximReference> refs = new LinkedList<LoximReference>();
		
		for (LoximObject item : pool.getObjects()) {
			if (item instanceof LoximReference) {
				refs.add(((LoximReference)item));
			}
		}
		con.beginTransaction();
		
		service.expandAll();
		
		con.commitTransation();
		
		
		System.out.println("Zawartosc pojemnika po wykonaniu zapytania");
		
		for (LoximObject item : pool.getObjects()) {
			System.out.println(item);
		}
	}
}
