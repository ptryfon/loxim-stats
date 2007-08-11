package pl.tzr.transparent;

import org.junit.Test;

import pl.tzr.transparent.data.Component;
import pl.tzr.transparent.data.ComponentImpl;
import pl.tzr.transparent.data.Part;
import pl.tzr.transparent.data.PartImpl;

/**
 * Automated tests of persisting Java objects by transparent mapping layer
 * @author Tomasz Rosiek
 *
 */
public class CreateTest extends AbstractTransparentTestCase {
        
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
    	
		
	}		    
    
    /**
     * Tests addition of existing object as a composite attribute of some object
     */
    @Test
    public void testCreateObject3() throws Exception {

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
        
        /* Create component in root */
        Component component1 = new ComponentImpl();
        component1.setAmount(3);
        component1.setLeadsTo(persistentChildPart);
        
        Component persistentComponent1 = 
            (Component)transparentSession.persist(component1);
        
        /* Bind the component as child of part */        
        persistentPart.setSingleComponent(persistentComponent1);
                        
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
        
        
    }    

}
