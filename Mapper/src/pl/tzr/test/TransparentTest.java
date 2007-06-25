package pl.tzr.test;

import javax.xml.ws.ServiceMode;

import org.junit.Test;

import junit.framework.TestCase;
import pl.tzr.driver.loxim.LoximDatasourceImpl;
import pl.tzr.exception.TransparentDeletedException;
import pl.tzr.test.data.Component;
import pl.tzr.test.data.ComponentImpl;
import pl.tzr.test.data.Part;
import pl.tzr.test.data.PartImpl;
import pl.tzr.transparent.SimpleTransparentSessionFactoryImpl;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.TransparentSessionFactory;

public class TransparentTest extends TestCase {
    
    /* FIXME - make proper assertions for that */
    
    TransparentSessionFactory transparentSessionFactory;
    
    TransparentSession transparentSession;
	    
	@Override
    protected void setUp() throws Exception {
        
        /* Create a datasource */
        LoximDatasourceImpl datasource = new LoximDatasourceImpl();
        datasource.setHost("127.0.0.1");
        datasource.setPort(6543);
        datasource.setLogin("root");
        datasource.setPassword("");     

        /* Create a transparent session factory */
        
        transparentSessionFactory = new SimpleTransparentSessionFactoryImpl(
                datasource, new Class[] { Part.class, Component.class });
        
        /* Start new session */     
        transparentSession = transparentSessionFactory.getCurrentSession();        

    }

    @Override
    protected void tearDown() throws Exception {
        transparentSessionFactory.closeCurrentSession();
    }

	/**
	 * Tests creation of object with simple and composite attributes 
	 */
    @Test
	public void testCreateObject() throws Exception {
		
		/* Create the first part */
		Part childPart = new PartImpl();
		childPart.setDetailCost(2);
		childPart.setDetailMass(2);
		childPart.setKind("normal");
		childPart.setName("Screw");						
		
		Part persistentChildPart = (Part)transparentSession.persist(childPart);
					
		/* Create the second part */
		Part part = new PartImpl();
		part.setDetailCost(30);
		part.setDetailMass(77);
		part.setKind("normal");
		part.setName("Tail");
		
		/* Create component of second part containing reference to the first part */
		Component component1 = new ComponentImpl();
		component1.setAmount(3);
		component1.setLeadsTo(persistentChildPart);
		part.setSingleComponent(component1);
		
		Part persistentPart = (Part)transparentSession.persist(part);
								
				
		/* Access to created part */															
		System.out.println(persistentPart.getName());
		System.out.println(persistentPart.getKind());
		System.out.println(persistentPart.getDetailCost());
		
		/* Modify simple property of created part */
		persistentPart.setDetailCost(88);
		
		/* Display properties of the component */		
		System.out.println("component amount=" + persistentPart.getSingleComponent().getAmount());
		System.out.println("component leads to=" + persistentPart.getSingleComponent().getLeadsTo().getName());
								
		transparentSession.commit();
		
	}
	
	/**
	 * Tests addition of simple and composite attributes to existing object
	 */
    @Test
	public void testCreateObject2() throws Exception {

    	/* Create the first part */
    	Part childPart = new PartImpl();
    	childPart.setDetailCost(2);
    	childPart.setDetailMass(2);
    	childPart.setKind("normal");
    	childPart.setName("Screw");						
    	
    	Part persistentChildPart = (Part)transparentSession.persist(childPart);
    				
    	/* Create the second part */
    	Part part = new PartImpl();
    	part.setName("Tail");
    							
    	Part persistentPart = (Part)transparentSession.persist(part);
    	
    	/* Set new properties of the persisted object */
    	persistentPart.setDetailCost(30);
    	persistentPart.setDetailMass(77);
    	persistentPart.setKind("normal");
    	
    	/* Create component as a child of persisted object */
    	Component component1 = new ComponentImpl();
    	component1.setAmount(3);
    	component1.setLeadsTo(persistentChildPart);
    	persistentPart.setSingleComponent(component1);
    					
    	/* Access to created part */															
    	System.out.println(persistentPart.getName());
    	System.out.println(persistentPart.getKind());
    	System.out.println(persistentPart.getDetailCost());
    	
    	/* Modify simple property of created part */
    	persistentPart.setDetailCost(88);
    	
    	/* Display properties of the component */		
    	System.out.println("component amount=" + persistentPart.getSingleComponent().getAmount());
    	System.out.println("component leads to=" + persistentPart.getSingleComponent().getLeadsTo().getName());
    	
    	transparentSession.commit();
		
	}
	
	/**
	 * Tests deletion of object's primitive attribute 
	 */
    @Test
	public void testDeleteObject1() throws Exception {
											
		Part part = new PartImpl();

		part.setDetailCost(30);
		part.setDetailMass(77);
		part.setKind("normal");
		part.setName("Tail");								
		
		Part persistentPart = (Part)transparentSession.persist(part);
								
		System.out.println("Before attribute deletion:" + persistentPart.getKind());
		persistentPart.setKind(null);
		System.out.println("After attribute deletion:" + persistentPart.getKind());
		System.out.println(persistentPart.getDetailCost());
											
		transparentSession.commit();

	}	
	
	/**
	 * Tests deletion of object's composite property 
	 */
    @Test
	public void testDeleteObject2() throws Exception {
		
									
		Part part = new PartImpl();

		part.setDetailCost(30);
		part.setDetailMass(77);
		part.setKind("normal");
		part.setName("Tail");
		
		Component comp = new ComponentImpl();
		comp.setAmount(33);
		comp.setLeadsTo(null);
		part.setSingleComponent(comp);
		
		Part persistentPart = (Part)transparentSession.persist(part);
		
		Component persistentComp = persistentPart.getSingleComponent();
								
		System.out.println("Before attribute deletion:" + persistentComp.getAmount());
		persistentPart.setSingleComponent(null);
		System.out.println("After attribute deletion (should be null):" + persistentPart.getSingleComponent());
		
		try {
			/* persistentComp has been deleted - exception is thrown when you try to access it */ 
			persistentComp.getAmount();
			throw new IllegalStateException();
		} catch (TransparentDeletedException e) {
			System.out.println("Properly throw of the exception");	
		}
		
		transparentSession.commit();

		
	}
	
	/**
	 * Tests deletion of an object wich is referenced from other object 
	 */
    @Test
	public void testDeleteReferenceTarget() throws Exception {
		
		Part part2 = new PartImpl();			
		part2.setDetailCost(30);
		part2.setDetailMass(77);
		part2.setKind("normal");
		part2.setName("Tail");

		Part persistentPart2 = (Part)transparentSession.persist(part2);			
								
		Part part = new PartImpl();

		part.setDetailCost(30);
		part.setDetailMass(77);
		part.setKind("normal");
		part.setName("Tail");
		
		Component comp = new ComponentImpl();
		comp.setAmount(33);
		comp.setLeadsTo(null);
		part.setSingleComponent(comp);
		
		Part persistentPart = (Part)transparentSession.persist(part);
		
		Component persistentComp = persistentPart.getSingleComponent();
		
		persistentComp.setLeadsTo(persistentPart2);
								
		System.out.println("Before reference target deletion:" + persistentComp.getLeadsTo());
		
		transparentSession.delete(persistentPart2);
		
		System.out.println("After attribute deletion (should be null):" + persistentComp.getLeadsTo());
		
		try {
			
			persistentPart2.getDetailCost();
			throw new IllegalStateException();
		} catch (TransparentDeletedException e) {
			System.out.println("Properly throw of the exception");	
		}
																	
		transparentSession.commit();

		
	}	
    
    /**
     * Tests operations on the set
     * @throws Exception
     */
    @Test
    public void testSet() throws Exception {
        
        Part part = new PartImpl();            
        part.setDetailCost(30);
        part.setDetailMass(77);
        part.setKind("normal");
        part.setName("Tail");

        Part persistentPart = (Part)transparentSession.persist(part);
        
        Component c1 = new ComponentImpl();
        c1.setAmount(1);
        
        Component c2 = new ComponentImpl();
        c2.setAmount(1);
        
        persistentPart.getComponent().add(c1);
        
        persistentPart.getComponent().add(c2);
        
        for (Component c : persistentPart.getComponent()) {
            
            System.out.println(c.getAmount());
            
        }
        
        Component c = persistentPart.getComponent().iterator().next();
        
        assertTrue(persistentPart.getComponent().contains(c));
        
        assertEquals(2, persistentPart.getComponent().size());
        
        persistentPart.getComponent().remove(c);
        
        assertFalse(persistentPart.getComponent().contains(c));
        
        assertEquals(1, persistentPart.getComponent().size());
        
    }
	
	

//	private void testTransparentProxying() throws Exception {
//						
//		/* Start new session */
//		
//		TransparentSession transparentSession = transparentSessionFactory.getCurrentSession();
//		
//		try {			
//
//			Set<Object> results = transparentSession
//					.find("Part", Part.class);
//
//			/* Odczytujemy wszystkie elementy typu Part */
//
//			for (Object item : results) {
//
//				Part part = (Part) item;
//
//				System.out.println("Part name=" + part.getName());
//
//				System.out.println("Components of the part:");
//
//				if (part.getComponent() != null) {
//
//					
//					for (Component comp : part.getComponent()) {
//						System.out.println("- component leadsTo "
//								+ comp.getLeadsTo().getName());
//						System.out.println(" -component amount ="
//								+ comp.getAmount());
//
//						/* Dokonujemy modyfikacji bazy */
//						comp.setAmount(comp.getAmount() + 1);
//
//					}
//				
//				}
//
//				System.out.println();
//
//			}
//			
//			transparentSession.commit();
//
//		} catch (Exception e) {
//			
//			transparentSession.rollback();
//			
//			throw e;
//
//		}
//		
//		
//
//	}




}
