package pl.tzr.test;

import java.util.Set;

import pl.tzr.browser.session.Session;
import pl.tzr.browser.session.SessionImpl;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.TcpConnectionFactory;
import pl.tzr.test.data.Component;
import pl.tzr.test.data.Part;
import pl.tzr.transparent.JavaTransparentProxyFactory;
import pl.tzr.transparent.TransparentProxyFactory;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.TransparentSessionImpl;
import pl.tzr.transparent.proxy.handler.registry.AccessorRegistryFactory;
import pl.tzr.transparent.proxy.handler.registry.SimpleAccessorRegistryFactory;
import pl.tzr.transparent.structure.model.ModelRegistry;
import pl.tzr.transparent.structure.model.SimpleAnnotatedModelRegistryFactory;

public class TransparentTest {
		
	
	
	private static void testTransparentProxying() throws Exception {
		
		SimpleConnection connection;
		
		/* Tworzymy polaczenie z baza daych */
		
		connection = new SimpleConnection(
				TcpConnectionFactory.getConnection("127.0.0.1", 6543));
				
		
		connection.login("root", "");		
		connection.beginTransaction();
		
		try {
		
			TransparentSession transparentSession = createTestSession(connection);
					
			Set<Object> results = transparentSession.find("Part", Part.class);
			
			/* Odczytujemy wszystkie elementy typu Part */
			
			for (Object item : results) {
				
				Part part = (Part)item;
				
				System.out.println("Part name=" + part.getName());
				
				System.out.println("Components of the part:");
				
				if (part.getComponent() != null) {
					
					for (Component comp : part.getComponent()) {
						System.out.println("- component leadsTo " + comp.getLeadsTo().getName());
						System.out.println(" -component amount =" + comp.getAmount());
						
						/* Dokonujemy modyfikacji bazy */
						comp.setAmount(comp.getAmount() + 1);
						
					}
					
					
				}
				
				System.out.println();
									
			}	
			
			connection.commitTransation();
		
		} catch (Exception e) {
			
			connection.rollbackTransaction();
			connection.close();
			
			throw e;
			
		}
		
	}
	
	private static TransparentSession createTestSession(
			SimpleConnection connection) throws Exception {
		
		
		/* Tworzymy sesje nawigatora */ 
		Session session = new SessionImpl(connection);
					
		/* Tworzymy fabryke modeli */
		SimpleAnnotatedModelRegistryFactory modelRegistryFactory = 
			new SimpleAnnotatedModelRegistryFactory();			
		
		/* Tworzymy fabryke rejestru typow danych */
		AccessorRegistryFactory accessorRegistryFactory = new SimpleAccessorRegistryFactory();
				
		/* Okreslamy jakie klasy maja sie skladac na model */
		modelRegistryFactory.setClasses(new Class[] {Part.class, Component.class});
		
		/* Tworzymy rejestr typow danych */
		modelRegistryFactory.setHandlerRegistry(accessorRegistryFactory.getHandlerRegistry());
		
		/* Tworzymy model */
		ModelRegistry modelRegistry = modelRegistryFactory.getModelRegistry();
		
		/* Tworzymy fabryke przezroczystych obiektow */
		TransparentProxyFactory transparentProxyFactory = 
			new JavaTransparentProxyFactory(modelRegistry);

		/* Otwieramy sesje mappera */
		TransparentSession transparentSession = 
			new TransparentSessionImpl(session, transparentProxyFactory, modelRegistry);

		return transparentSession;
		
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception {			
		
		testTransparentProxying();
		
	}

}
