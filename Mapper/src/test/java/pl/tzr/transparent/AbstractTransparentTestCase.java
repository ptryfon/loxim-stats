package pl.tzr.transparent;

import junit.framework.TestCase;
import pl.tzr.browser.session.LoximSession;
import pl.tzr.driver.loxim.LoximDatasourceImpl;
import pl.tzr.transparent.data.Component;
import pl.tzr.transparent.data.Part;

/**
 * Abstract class for all automated tests of the transparent
 * mapping layer
 * @author Tomasz Rosiek
 *
 */
public abstract class AbstractTransparentTestCase extends TestCase {

    protected SimpleTransparentSessionFactoryImpl transparentSessionFactory;
    
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
        
        transparentSessionFactory = new SimpleTransparentSessionFactoryImpl();
        
        transparentSessionFactory.setClasses(new Class[] { Part.class, Component.class });
        
        transparentSessionFactory.setDatasource(datasource);
        
        /* Start new session */     
        transparentSession = transparentSessionFactory.getSession();
        
        ((LoximSession)transparentSession.getSession()).getConnection().execute("delete PART");
        ((LoximSession)transparentSession.getSession()).getConnection().execute("delete Component");
    
    }

    @Override
    protected void tearDown() throws Exception {
        if (transparentSession.isActive()) {
            transparentSession.rollback();        
        }
    }

}