/**
 * 
 */
package pl.tzr.transparent;

import pl.tzr.browser.session.LoximSession;
import pl.tzr.driver.loxim.LoximDatasourceImpl;
import pl.tzr.transparent.data.Component;
import pl.tzr.transparent.data.Part;
import pl.tzr.transparent.data.PartImpl;
import junit.framework.TestCase;

/**
 * @author Tomasz Rosiek
 *
 */
public class ReattachTest extends TestCase {
    
    protected SimpleTransparentSessionFactoryImpl transparentSessionFactory;
    
    @Override
    protected void setUp() throws Exception {
        
        /* Create a datasource */
        LoximDatasourceImpl datasource = new LoximDatasourceImpl();
        datasource.setHost("127.0.0.1");
        datasource.setPort(6543);
        datasource.setLogin("root");
        datasource.setPassword("");
        
        /* Create a transparent session factory */
        transparentSessionFactory = new SimpleTransparentSessionFactoryImpl();
        
        transparentSessionFactory.setClasses(new Class[] { Part.class, Component.class });
        
        transparentSessionFactory.setDatasource(datasource);
        
        /* Start new session and delete existing objects */     
        TransparentSession transparentSession = 
            transparentSessionFactory.getSession();
        
        ((LoximSession)transparentSession.getSession()).getConnection().execute("delete PART");
        ((LoximSession)transparentSession.getSession()).getConnection().execute("delete Component");
        
        transparentSession.commit();
        
    }
    
    public void testReattaching() {
        
        TransparentSession transparentSession = 
            transparentSessionFactory.getSession();
        
        String partRef = null;

        try {
            
            /* Create the first part */
            Part part = new PartImpl();
            part.setDetailCost(2);
            part.setDetailMass(2);
            part.setKind("normal");
            part.setName("Screw");                     
            
            Part persistentPart = (Part)transparentSession.persist(part);
            
            partRef = transparentSession.getId(persistentPart);           
            
            transparentSession.commit();
            
        } catch (RuntimeException e) {
            transparentSession.rollback();
            throw e;
        }
        
        System.out.println(partRef);
        
        TransparentSession transparentSession2 = 
            transparentSessionFactory.getSession();
        
        try {
            
            Part refetchedPart = (Part)transparentSession2.getById(partRef, Part.class);
            
            assertEquals("Screw", refetchedPart.getName());
            
            transparentSession2.commit();
            
        } catch (RuntimeException e) {
            transparentSession2.rollback();
            throw e;
        }        
        
    }
        

}
