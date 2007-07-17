package pl.tzr.transparent;

import java.util.Collection;

import org.junit.Test;

import pl.tzr.transparent.data.Part;
import pl.tzr.transparent.data.PartImpl;

/**
 * Automated tests of running queries in the transparent mapping layer
 * @author Tomasz Rosiek
 *
 */
public class QueryTest extends AbstractTransparentTestCase {
        
    
    @Test
    public void testParametrizedQueryPrimitiveParam() throws Exception {
        
        final int DESIRED_DETAIL_MASS = 875;
        
        /* Create the first part */
        Part part1 = new PartImpl();
        part1.setDetailCost(30);
        part1.setDetailMass(DESIRED_DETAIL_MASS);
        part1.setKind("normal");
        part1.setName("Tail");
        
        /* Create the first part */
        Part part2 = new PartImpl();
        part2.setDetailCost(88);
        part2.setDetailMass(77);
        part2.setKind("normal");
        part2.setName("Part2");
        
        transparentSession.persist(part1);
        transparentSession.persist(part2);
        
        
        Collection<Part> results = transparentSession.findWithParams(
                "PART where detailMass = ?", Part.class, DESIRED_DETAIL_MASS);
        
        assertTrue(results.size() > 0);
        
        for (Part result : results) {
            
            assertEquals(DESIRED_DETAIL_MASS, result.getDetailMass());
            
        }
        
        Collection<Part> results2 = transparentSession.findWithParams(
                "PART where detailMass != ?", Part.class, DESIRED_DETAIL_MASS);
        
        assertTrue(results.size() > 0);
        
        boolean wasPart2 = false;
        
        for (Part result : results2) {
            
            if (result.getName().equals("Part2") && result.getDetailMass() == 77) wasPart2 = true;            
            
        }        
        
        assertTrue(wasPart2);
                
 
    }
	
    @Test
    public void testParametrizedQueryObjectParam() throws Exception {

        final int DESIRED_DETAIL_MASS = 875;
        
        /* Create the first part */
        Part part1 = new PartImpl();
        part1.setDetailCost(30);
        part1.setDetailMass(DESIRED_DETAIL_MASS);
        part1.setKind("normal");
        part1.setName("Tail");
        
        /* Create the first part */
        Part part2 = new PartImpl();
        part2.setDetailCost(88);
        part2.setDetailMass(77);
        part2.setKind("normal");
        part2.setName("Part2");
        
        Part persistentPart1 = (Part)transparentSession.persist(part1);
        transparentSession.persist(part2);
        
        
        Collection<Part> results = transparentSession.findWithParams(
                "PART intersect ?", Part.class, persistentPart1);
        
        assertTrue(results.size() == 1);
        
        for (Part result : results) {
            
            assertEquals(DESIRED_DETAIL_MASS, result.getDetailMass());
            assertEquals("Tail", result.getName());
            
        }
                                
    }
    
    @Test
    public void testPrimitiveQuery() throws Exception {
        
        final int DESIRED_DETAIL_MASS = 875;
        
        /* Create the first part */
        Part part1 = new PartImpl();
        part1.setDetailCost(30);
        part1.setDetailMass(DESIRED_DETAIL_MASS);
        part1.setKind("normal");
        part1.setName("Tail");
        
        /* Create the first part */
        Part part2 = new PartImpl();
        part2.setDetailCost(88);
        part2.setDetailMass(77);
        part2.setKind("normal");
        part2.setName("Part2");
        
        Part persistentPart1 = (Part)transparentSession.persist(part1);
        transparentSession.persist(part2);
        
        
        Collection<Object> results = transparentSession.findPrimitiveWithParams(
                "count(PART intersect ?)", persistentPart1);
        
        assertTrue(results.size() == 1);
        
        Object firstResult = results.iterator().next();
        
        assertEquals(1, firstResult);
        
    }
    

}
