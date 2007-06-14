package pl.tzr.transparent;

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
	
	private final LoximDatasource datasource;
	
	private final Class[] classes;
	
	protected TransparentSession currentSession;
	
	public SimpleTransparentSessionFactoryImpl(
			final LoximDatasource datasource, 
			final Class[] classes) {
		this.datasource = datasource;
		this.classes = classes;
	}		

	public void closeCurrentSession() {
		
		if ((currentSession != null) && (currentSession.isActive())) {
			currentSession.rollback();
		}

		
	}

	public TransparentSession getCurrentSession() {
		if ((currentSession == null) || (!currentSession.isActive())) {
			currentSession = createNewSession();
		}

		return currentSession;
	}
	
	protected TransparentSession createNewSession() {
		
		Session session;
		
		try {
			session = new LoximSession(datasource.getConnection());
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		}

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
		
		DatabaseContext context = new DatabaseContext();
		context.setModelRegistry(modelRegistry);
		context.setTransparentProxyFactory(transparentProxyFactory);

		/* Create mapper session */
		TransparentSession transparentSession = new TransparentSessionImpl(
				session, context);

		return transparentSession;

	}
	

}
