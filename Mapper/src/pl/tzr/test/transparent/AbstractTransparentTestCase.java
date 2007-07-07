package pl.tzr.test.transparent;

import pl.tzr.browser.session.LoximSession;
import pl.tzr.driver.loxim.LoximDatasourceImpl;
import pl.tzr.test.data.Component;
import pl.tzr.test.data.Part;
import pl.tzr.transparent.SimpleTransparentSessionFactoryImpl;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.TransparentSessionFactory;
import junit.framework.TestCase;

/**
 * Abstract class for all automated tests of the transparent
 * mapping layer
 * @author Tomasz Rosiek
 *
 */
public abstract class AbstractTransparentTestCase extends TestCase {

    protected TransparentSessionFactory transparentSessionFactory;
    
    protected TransparentSession transparentSession;


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
        
        ((LoximSession)transparentSession.getSession()).getConnection().execute("delete PART");
        ((LoximSession)transparentSession.getSession()).getConnection().execute("delete Component");
    
    }

    @Override
    protected void tearDown() throws Exception {
        transparentSessionFactory.closeCurrentSession();
    }

}