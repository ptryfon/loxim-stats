package pl.tzr.test;

import java.util.HashSet;
import java.util.Set;

import junit.framework.TestCase;

import org.junit.Test;

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
        
        assertNotNull(persistentPart);
        
        /* Access to created part */
        assertEquals("Tail", persistentPart.getName());
        assertEquals("normal", persistentPart.getKind());
        assertEquals(30, persistentPart.getDetailCost());
        assertEquals(77, persistentPart.getDetailMass());
								
		
		/* Modify simple property of created part */
		persistentPart.setDetailCost(88);
		
		/* Access properties of the component */
        assertEquals(3, persistentPart.getSingleComponent().getAmount());
        assertEquals("Screw", persistentPart.getSingleComponent().getLeadsTo().getName());
        								
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
        assertEquals("Tail", persistentPart.getName());
        assertEquals("normal", persistentPart.getKind());
        assertEquals(30, persistentPart.getDetailCost());
        assertEquals(77, persistentPart.getDetailMass());
    	
    	/* Modify simple property of created part */
    	persistentPart.setDetailCost(88);
    	
    	/* Display properties of the component */		
        assertEquals(3, persistentPart.getSingleComponent().getAmount());
        assertEquals("Screw", persistentPart.getSingleComponent().getLeadsTo().getName());
    	
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
        
        assertEquals("normal", persistentPart.getKind());								
		persistentPart.setKind(null);
        assertEquals(null, persistentPart.getKind());
		assertEquals(30, persistentPart.getDetailCost());
											
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
								
        assertEquals(33, persistentComp.getAmount());
		persistentPart.setSingleComponent(null);
        assertNull(persistentPart.getSingleComponent());		
		
		try {			 
			persistentComp.getAmount();
			fail();
		} catch (TransparentDeletedException e) {
            /* persistentComp has been deleted
             * An exception is thrown when you try to access it */
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
        
        assertEquals("Tail", persistentComp.getLeadsTo().getName());
												
		transparentSession.delete(persistentPart2);
        
        assertNull(persistentComp.getLeadsTo());
				
		try {
			
			persistentPart2.getDetailCost();
            fail();

		} catch (TransparentDeletedException e) {
            /* An exception thrown properly*/
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
        c2.setAmount(2);
        
        persistentPart.getComponent().add(c1);
        
        persistentPart.getComponent().add(c2);
        
        
        boolean wasC1 = false;
        boolean wasC2 = false;
        
        for (Component c : persistentPart.getComponent()) {
            
            if (c.getAmount() == 1) wasC1 = true;
            if (c.getAmount() == 2) wasC2 = true;
            
            
        }
        
        assertTrue(wasC1 && wasC2);
        
        Component c = persistentPart.getComponent().iterator().next();
        
        assertTrue(persistentPart.getComponent().contains(c));
        
        assertEquals(2, persistentPart.getComponent().size());
        
        persistentPart.getComponent().remove(c);
        
        assertFalse(persistentPart.getComponent().contains(c));
        
        assertEquals(1, persistentPart.getComponent().size());
        
    }
    
    @Test 
    public void testReferenceSet() throws Exception {
        
        Part part = new PartImpl();            
        part.setDetailCost(30);
        part.setDetailMass(77);
        part.setKind("normal");
        part.setName("Tail");

        Part persistentPart = (Part)transparentSession.persist(part);
        
        Set<Component> transientComponents = new HashSet<Component>();
        
        Component comp1 = new ComponentImpl();
        comp1.setAmount(1);        
        Component persistentComp1 = (Component)transparentSession.persist(comp1);
        
        Component comp2 = new ComponentImpl();
        comp2.setAmount(2);        

        Component persistentComp2 = (Component)transparentSession.persist(comp2);
        
        /* Add to transient set */
        transientComponents.add(persistentComp1);
        transientComponents.add(persistentComp2);
        
        persistentPart.setReferenceComponent(transientComponents);
        
        //
        
        Set<Component> persistentSet = persistentPart.getReferenceComponent(); 
        
        assertNotNull(persistentSet);
        
        //assertFalse(persistentPart.getReferenceComponent().isEmpty());
        
        assertEquals(2, persistentPart.getReferenceComponent().size());
        
        /* Add to persistent set */
        Component comp3 = new ComponentImpl();
        comp3.setAmount(3);
        Component persistentComp3 = (Component)transparentSession.persist(comp3);
        
        persistentSet.add(persistentComp3);
        
        assertEquals(3, persistentPart.getReferenceComponent().size());
        
        //transparentSession.commit();
        
        boolean wasComp1 = false;
        boolean wasComp2 = false;
        boolean wasComp3 = false;                
        
        for (Component comp : persistentPart.getReferenceComponent()) {
            
            switch (comp.getAmount()) {
            case 1: 
                wasComp1 = true;
                break;
            case 2:
                wasComp2 = true;
                break;
            case 3:
                wasComp3 = true;
                break;
            default:
                fail();
            }

        }
        
        assertTrue(wasComp1);
        assertTrue(wasComp2);
        assertTrue(wasComp3);
        
        assertTrue(persistentSet.contains(persistentComp2));
        
        persistentSet.remove(persistentComp2);
        
        assertFalse(persistentSet.contains(persistentComp2));
        
        assertEquals(2, persistentSet.size());
               
    }
    
    @Test
    public void testPrimitiveSet() {
        /* TODO */
    }	
	
//TODO Write tests of querying


}
