package pl.tzr.transparent;

import pl.tzr.browser.session.Session;
import pl.tzr.browser.session.LoximSession;
import pl.tzr.driver.loxim.SimpleConnection;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.transparent.proxy.handler.registry.AccessorRegistryFactory;
import pl.tzr.transparent.proxy.handler.registry.SimpleAccessorRegistryFactory;
import pl.tzr.transparent.structure.model.ModelRegistry;
import pl.tzr.transparent.structure.model.SimpleAnnotatedModelRegistryFactory;

public class SimpleTransparentSessionFactoryImpl implements
		TransparentSessionFactory {
	
	private final SimpleConnection connection;
	private final Class[] classes;
	
	protected TransparentSession currentSession;
	
	public SimpleTransparentSessionFactoryImpl(final SimpleConnection connection, Class[] classes) {
		this.connection = connection;
		this.classes = classes;
	}		

	public void closeCurrentSession() {
		
		if ((currentSession != null) && (currentSession.isActive())) {
			currentSession.rollback();
		}
		
	}

	public TransparentSession getCurrentSession() {
		if ((currentSession != null) && (currentSession.isActive())) {
			currentSession = createNewSession();
		}

		return currentSession;
	}
	
	protected TransparentSession createNewSession() {
		
		Session session;
		
		try {
			session = new LoximSession(connection);
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
		TransparentProxyFactory transparentProxyFactory = new JavaTransparentProxyFactory(
				modelRegistry);

		/* Otwieramy sesje mappera */
		TransparentSession transparentSession = new TransparentSessionImpl(
				session, transparentProxyFactory);

		return transparentSession;

	}
	

}
