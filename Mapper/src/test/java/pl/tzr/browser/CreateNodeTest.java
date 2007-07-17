package pl.tzr.browser;

import java.util.Collection;

import pl.tzr.browser.store.node.ComplexValue;
import pl.tzr.browser.store.node.LoximNode;
import pl.tzr.browser.store.node.Node;
import pl.tzr.browser.store.node.SimpleValue;

/**
 * Automated tests of browsing layer, creating nodes 
 * @author Tomasz Rosiek
 *
 */
public class CreateNodeTest extends AbstractNodeTestCase {
    

    public void testCreateObjectInRoot() throws Exception {
        
        Node newPart = new LoximNode("PART", new ComplexValue());       
        newPart.addChild(new LoximNode("price", new SimpleValue(250)));        
        newPart.addChild(new LoximNode("price", new SimpleValue(275)));
        newPart.addChild(new LoximNode("name", new SimpleValue("Schodki")));        
        session.addToRoot(newPart);
        
        assertNotNull(newPart.getReference());
        
        Collection<SimpleValue> prices = session.findPrimitive("deref(PART.price)");
        
        assertTrue(2 < prices.size());
        
        boolean wasPrice250 = false;
        boolean wasPrice275 = false;
        
        for (SimpleValue price : prices) {
            
            System.out.println(price.getInteger());
            System.out.println(Integer.toBinaryString(price.getInteger()));
            
            if (price.getInteger().intValue() == 250) wasPrice250 = true;
            if (price.getInteger().intValue() == 275) wasPrice275 = true;
            
        }
        
        assertTrue(wasPrice250 && wasPrice275);
                      
    }


}
