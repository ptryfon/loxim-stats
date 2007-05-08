package pl.tzr.test;

import java.util.Collection;

import junit.framework.TestCase;
import pl.tzr.browser.store.LoximStore;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.Connection;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.TcpConnectionFactory;

/**
 * <br/>
 * UWAGA! Test wymaga uruchomionego serwera LoXiM
 * @author Tomasz Rosiek
 *
 */
public class LoximStoreTest extends TestCase {
	
	Connection connection;
	
	SimpleConnection simpleConnection;
	
	LoximStore store; 	

	@Override
	protected void setUp() throws Exception {

		connection = TcpConnectionFactory.getConnection("127.0.0.1", 6543);
		simpleConnection = new SimpleConnection(connection);
		
		simpleConnection.login("root", "");		
		simpleConnection.beginTransaction();
		
		store = new LoximStore(simpleConnection);
		
	}

	@Override
	protected void tearDown() throws Exception {
		
		simpleConnection.rollbackTransaction();		
		connection.close();	
		
	}
	
	public void testFetchSimpleObjectFromRoot() throws Exception {
		
		//Tworzymy prosty obiekt
		connection.execute("create 5 as simple");
		
		//Próbujemy go pobrać za pomocą mappera
		Collection<Node> results = store.executeQuery("simple");
		
		//Sprawdzamy czy otrzymaliśmy poprawny obiekt
		
		assertEquals(1, results.size());
		
		Node result = results.iterator().next();
		
		assertEquals("simple", result.getName());
		
		assertTrue(result.getValue() instanceof SimpleValue);
		
		SimpleValue value = (SimpleValue)result.getValue();
		
		assertEquals(SimpleValue.Type.INT, value.getType());
		
		assertEquals(new Integer(5), value.getInteger());
		
	}
	
	public void testFetchCompositeObjectFromRoot() {
		//TODO
	}
	
	public void testFetchChildOfComposite() {
		//TODO
	}
	
	public void testFetchReferenceTarget() {
		//TODO
	}
	
	public void testCreateSimpleObject() {
		//TODO
	}
	
	public void testCreateCompositeObject() {
		//TODO
	}
	
	public void testDeleteSimpleObject() {
		//TODO
	}
	
	public void testDeleteCompositeObject() {
		//TODO
	}
	
	public void testChangeSimpleObject() {
		//TODO
	}
	
	public void testAddPropertyToComposite() {
		//TODO
	}
	
	public void testMoveProperty() {
		//TODO
	}

}
