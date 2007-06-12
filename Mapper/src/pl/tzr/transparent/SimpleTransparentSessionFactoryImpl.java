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
	
	public SimpleTransparentSessionFactoryImpl(final LoximDatasource datasource, Class[] classes) {
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

		/* Tworzymy fabryke modeli */
		SimpleAnnotatedModelRegistryFactory modelRegistryFactory = new SimpleAnnotatedModelRegistryFactory();

		/* Tworzymy fabryke rejestru typow danych */
		AccessorRegistryFactory accessorRegistryFactory = new SimpleAccessorRegistryFactory();

		/* Okreslamy jakie klasy maja sie skladac na model */
		modelRegistryFactory.setClasses(classes);

		/* Tworzymy rejestr typow danych */
		modelRegistryFactory.setHandlerRegistry(accessorRegistryFactory
				.getHandlerRegistry());

		/* Tworzymy model */
		ModelRegistry modelRegistry = modelRegistryFactory.getModelRegistry();

		/* Tworzymy fabryke przezroczystych obiektow */
		TransparentProxyFactory transparentProxyFactory = new JavaTransparentProxyFactory();
		
		DatabaseContext context = new DatabaseContext();
		context.setModelRegistry(modelRegistry);
		context.setTransparentProxyFactory(transparentProxyFactory);

		/* Otwieramy sesje mappera */
		TransparentSession transparentSession = new TransparentSessionImpl(
				session, context);

		return transparentSession;

	}
	

}
