package pl.tzr.test.transparent;

import org.junit.Test;

import pl.tzr.exception.TransparentDeletedException;
import pl.tzr.test.data.Component;
import pl.tzr.test.data.ComponentImpl;
import pl.tzr.test.data.Part;
import pl.tzr.test.data.PartImpl;

/**
 * Automated tests of deleting objects in the transparent mapping layer
 * @author Tomasz Rosiek
 *
 */
public class DeleteTest extends AbstractTransparentTestCase {
        	
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
																	
		
	}	
    
    
    

}
