package pl.tzr.test;

import java.util.Collection;

import pl.tzr.browser.session.LoximSession;
import pl.tzr.browser.session.Session;
import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.driver.loxim.LoximDatasource;
import pl.tzr.driver.loxim.LoximDatasourceImpl;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.exception.DeletedException;

public class BrowserTest {
	
	private LoximDatasource datasource;
	
	public BrowserTest() {
		
		LoximDatasourceImpl newDatasource = new LoximDatasourceImpl();
		newDatasource.setHost("127.0.0.1");
		newDatasource.setPort(6543);
		newDatasource.setLogin("root");
		newDatasource.setPassword("");
		
		this.datasource = newDatasource;		
		
	}
	
	private void testCreateObject() throws Exception {
		
		SimpleConnection connection = datasource.getConnection();
		
		try {
			
			Session session = new LoximSession(connection);
			
			Node newPart = new LoximNode("part", new ComplexValue());
			
			newPart.addChild(new LoximNode("price", new SimpleValue(250)));
			
			newPart.addChild(new LoximNode("name", new SimpleValue("Schodki")));
			
			session.addToRoot(newPart);
			
			session.commit();
			
		} finally {
			
			datasource.release(connection);
					
		}
		
	}
	
	private void testDeleteRootNode() throws Exception {
		
		SimpleConnection connection = datasource.getConnection();
				
		try {
			
			Session session = new LoximSession(connection);
			
			Node newPart = new LoximNode("test", new ComplexValue());
						 						
			session.addToRoot(newPart);
									
			System.out.println("Value before deletion: " + newPart.getValue());
			
			newPart.delete();
						
			try {
								
				newPart.getValue();
				
				throw new IllegalStateException();
				
			} catch (DeletedException e) {
				
				System.out.println("Deletion successful - exception thrown");
				
			}
			
			session.commit();
			
		} finally {
			
			datasource.release(connection);
		
		
		}
		
		
	}
	
	private void testDeleteSubNode() throws Exception {
		
		SimpleConnection connection = datasource.getConnection();
		
		try {
			
			Session session = new LoximSession(connection);
			
			Node newPart = new LoximNode("test2", new ComplexValue());
			
			Node childNode = new LoximNode("child", new SimpleValue(30));
			
			newPart.addChild(childNode);
						 						
			session.addToRoot(newPart);			
			
			System.out.println("Child value before deletion: " + newPart.getUniqueChildNode("child").getValue());
			
			newPart.getUniqueChildNode("child").delete();
			
			
			System.out.println("Child after deletion (null expected): " + newPart.getUniqueChildNode("child"));
			
			try {
								
				childNode.getValue();
				
				throw new IllegalStateException();
				
			} catch (DeletedException e) {
				
				System.out.println("Usuniecie sie powiodlo");
				
			}
						
			session.commit();
			
			
		} finally {
			
			datasource.release(connection);
					
		}
	}
	
	private void testNavigation() throws Exception {
					
		SimpleConnection connection = datasource.getConnection();
		
		try {
					
			Session session = new LoximSession(connection);	
			
			Collection<Node> results = session.find("Part");
			
			int i = 0;
			
			for (Node result : results) {
				
				i++;
				
				System.out.println(
						"Node " + result.getName() + "@" + result.getReference());
				
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
			
			session.commit();
		
		} finally {
			
			datasource.release(connection);
		
		
		}
		
	}
	
	public static void main(String[] args) throws Exception {
		
		BrowserTest instance = new BrowserTest();
		
		//instance.testNavigation();
		
		//instance.testCreateObject();

		instance.testDeleteSubNode();

	}

}

