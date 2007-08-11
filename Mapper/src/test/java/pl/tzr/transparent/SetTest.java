package pl.tzr.transparent;

import java.util.HashSet;
import java.util.Set;

import org.junit.Test;

import pl.tzr.transparent.data.Component;
import pl.tzr.transparent.data.ComponentImpl;
import pl.tzr.transparent.data.Part;
import pl.tzr.transparent.data.PartImpl;

/**
 * Automated tests of operations on sets in the transparent mapping layer
 * @author Tomasz Rosiek
 *
 */
public class SetTest extends AbstractTransparentTestCase {
        
    
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
        
        Part part = new PartImpl();            
        part.setDetailCost(30);
        part.setDetailMass(77);
        part.setKind("normal");
        part.setName("Tail");
        
        part.getOtherNames().add("N1");        

        Part persistentPart = (Part)transparentSession.persist(part);

        persistentPart.getOtherNames().add("N2");
        
        persistentPart.getOtherNames().add("N3");
        
        assertEquals(3, persistentPart.getOtherNames().size());
        
        /* test adding item second time */
        
        persistentPart.getOtherNames().add("N1");
        
        assertEquals(3, persistentPart.getOtherNames().size());
        
        assertTrue(persistentPart.getOtherNames().contains("N1"));        
        
        persistentPart.getOtherNames().remove("N1");
        
        assertEquals(2, persistentPart.getOtherNames().size());
        
    }   
    
    @Test
    public void testClear() {
        
        Part part = new PartImpl();            
        part.setDetailCost(30);
        part.setDetailMass(77);
        part.setKind("normal");
        part.setName("Tail");
        
        part.getOtherNames().add("N1");        

        Part persistentPart = (Part)transparentSession.persist(part);

        persistentPart.getOtherNames().add("N2");
        
        persistentPart.getOtherNames().add("N3");
        
        assertEquals(3, persistentPart.getOtherNames().size());
        
        persistentPart.getOtherNames().clear();
                
        assertEquals(0, persistentPart.getOtherNames().size());
                
    }


}
