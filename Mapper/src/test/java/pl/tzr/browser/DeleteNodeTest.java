package pl.tzr.browser;

import org.junit.Test;

import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.SimpleValue;
import pl.tzr.exception.DeletedException;

/**
 * Automated tests of browsing layer, deleting nodes 
 * @author Tomasz Rosiek
 *
 */
public class DeleteNodeTest extends AbstractNodeTestCase {

    @Test
    public void testDeleteRootNode() throws Exception {
        
        Node newPart = new LoximNode("PART", new SimpleValue("test"));
                                            
        session.addToRoot(newPart);
        
        assertEquals("test", ((SimpleValue)newPart.getValue()).getValue());
                                
        newPart.delete();
                    
        try {
                            
            newPart.getValue();            
            fail();            
            
        } catch (DeletedException e) {
            /* Correctly thrown exception */
        }
        
               
    
    }
    
    @Test
    public void testDeleteSubNode() throws Exception {
        
        Node newPart = new LoximNode("PART", new ComplexValue());
        
        Node childNode = new LoximNode("child", new SimpleValue(30));
        
        newPart.addChild(childNode);
                                            
        session.addToRoot(newPart);
                
        assertEquals(30, ((SimpleValue)newPart.getUniqueChildNode("child").
                getValue()).getValue());
                
        newPart.getUniqueChildNode("child").delete();
        
        assertNull(newPart.getUniqueChildNode("child"));
        
        try {
                            
            childNode.getValue();
            
            fail();
            
        } catch (DeletedException e) {            
            /* Correctly thrown exception */            
        }

    }    
    
}
