package pl.tzr.browser;

import java.util.Collection;

import org.junit.Test;

import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.ObjectValue;
import pl.tzr.browser.store.node.ReferenceValue;
import pl.tzr.browser.store.node.SimpleValue;

/**
 * Automated tests of browsing layer, navigation through the database 
 * @author Tomasz Rosiek
 *
 */
public class NavigationNodeTest extends AbstractNodeTestCase {    
    
    @Test
    public void testNavigation() throws Exception {
        
        prepareData("/samolot-data.sbql");
                            
        Collection<Node> results = session.find("Part where name=\"Samolot\"");
        
        int i = 0;
        
        for (Node result : results) {
            
            i++;
            
            assertEquals("Part", result.getName());
            
            System.out.println(
                    "Node " + result.getName() + "@" + result.getReference());
            
            boolean wasSkrzydlo = false;
            boolean wasDziob = false;
            
            for (Node component : result.getChildNodes("component")) {
                
                assertEquals("component", component.getName());
                
                Node leadsTo = component.getUniqueChildNode("leadsTo");
                
                assertNotNull(leadsTo);
                
                ObjectValue value = leadsTo.getValue();
                System.out.println("REF:" + value);
                ReferenceValue ref = (ReferenceValue)value;
                
                Node targetNode = ref.getTargetNode();
                
                assertEquals("Part", targetNode.getName());
                
                ObjectValue targetNameValue = 
                    targetNode.getUniqueChildNode("name").getValue();
                
                if ("Skrzydlo".equals(((SimpleValue)targetNameValue).getValue())) 
                    wasSkrzydlo = true;
                
                if ("Dziob".equals(((SimpleValue)targetNameValue).getValue())) 
                    wasDziob = true;
                                
            }
            
            assertTrue(wasSkrzydlo && wasDziob);
            
            Node assemblyCost = result.getUniqueChildNode("assemblyCost");
            
            assertNotNull(assemblyCost);
            
            assemblyCost.setValue(new SimpleValue(i));            
            assertEquals(i, ((SimpleValue)assemblyCost.getValue()).getValue());
            
        }
        
        
    }    
    
}
