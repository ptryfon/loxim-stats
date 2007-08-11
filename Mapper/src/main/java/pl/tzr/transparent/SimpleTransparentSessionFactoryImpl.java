package pl.tzr.transparent;

import java.util.HashSet;
import java.util.Set;

import pl.tzr.browser.session.LoximSession;
import pl.tzr.browser.session.Session;
import pl.tzr.driver.loxim.LoximDatasource;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.transparent.proxy.handler.registry.AccessorRegistryFactory;
import pl.tzr.transparent.proxy.handler.registry.SimpleAccessorRegistryFactory;
import pl.tzr.transparent.structure.model.ModelRegistry;
import pl.tzr.transparent.structure.model.SimpleAnnotatedModelRegistryFactory;

public class SimpleTransparentSessionFactoryImpl implements
		TransparentSessionFactory {
	
	private LoximDatasource datasource;
	
	private Class[] classes;
    
    private DatabaseContext context;
    
    private boolean initialized = false;
	
	protected Set<TransparentSession> activeSessions = 
        new HashSet<TransparentSession>();
        
    protected void onInit() {                

        /* Create a factory of mapping definition */
        SimpleAnnotatedModelRegistryFactory modelRegistryFactory = 
            new SimpleAnnotatedModelRegistryFactory();

        /* Create a factory of property accessor registry */
        AccessorRegistryFactory accessorRegistryFactory = 
            new SimpleAccessorRegistryFactory();

        /* Define classes belonging to the model */
        modelRegistryFactory.setClasses(classes);

        /* Create a registry of property accessors */
        modelRegistryFactory.setHandlerRegistry(accessorRegistryFactory
                .getHandlerRegistry());

        /* Create a model registry */
        ModelRegistry modelRegistry = modelRegistryFactory.getModelRegistry();

        /* Create a factory of proxy objects */
        TransparentProxyFactory transparentProxyFactory = 
            new JavaTransparentProxyFactory();
        
        context = new DatabaseContext();
        context.setModelRegistry(modelRegistry);
        context.setTransparentProxyFactory(transparentProxyFactory);        
        
    }
    
    protected void initializeIfRequired() {
        if (!initialized) {
            onInit();
            initialized = true;
        }
    }
    
	
	public TransparentSession getSession() {
        
        initializeIfRequired();
        
		
		Session session;
		
		try {
			session = new LoximSession(datasource);
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}
		
		/* Create mapper session */
		TransparentSession transparentSession = new TransparentSessionImpl(
				session, context);

		return transparentSession;

	}


    public void setClasses(Class[] classes) {
        this.classes = classes;
    }
    
    public void setClassNames(Set<String> classNames) {
        
        this.classes = new Class[classNames.size()];
        
        int i = 0;
        
        try {
        
        for (String className : classNames) {
            this.classes[i] = Class.forName(className);
            i++;
        }
        
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
        
    }


    public void setDatasource(LoximDatasource datasource) {
        this.datasource = datasource;
    }
	

}
