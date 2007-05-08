package pl.tzr.test;

import java.util.Collection;

import pl.tzr.browser.session.Session;
import pl.tzr.browser.session.SessionImpl;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.TcpConnectionFactory;

public class BrowserTest {
	
	private static void testNavigation() throws Exception {
		
		SimpleConnection connection = new SimpleConnection(
				TcpConnectionFactory.getConnection("127.0.0.1", 6543));
		
		connection.login("root", "");		
		connection.beginTransaction();
		
		Session session = new SessionImpl(connection);
		
		Collection<Node> results = session.find("Part");
		
		int i = 0;
		
		for (Node result : results) {
			
			i++;
			
			System.out.println("Node " + result.getName() + "@" + result.getReference());
			for (Node component : result.getChildNodes("component")) {
				System.out.println("Subnode " + result.getName() + "@" + result.getReference());
				Node leadsTo = component.getUniqueChildNode("leadsTo");
				
				if (leadsTo != null) {
					ObjectValue value = leadsTo.getValue();
					System.out.println("REF:" + value);
					ReferenceValue ref = (ReferenceValue)value;
					
					System.out.println("TARGET:" + ref.getTargetNode().getName());
				}
				
			}
			
			Node detailCost = result.getUniqueChildNode("detailCost");
			
			if (detailCost != null) detailCost.setValue(new SimpleValue(i));
		}
		
		connection.commitTransation();
		
		connection.close();
		
	}
	

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception {
		
		testNavigation();

	}

}
