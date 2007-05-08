package pl.tzr.transparent.proxy;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Proxy;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.transparent.TransparentProxyFactory;
import pl.tzr.transparent.structure.model.ClassInfo;
import pl.tzr.transparent.structure.model.ModelRegistry;
import pl.tzr.transparent.structure.model.PropertyInfo;

/**
 * Implementacja handlera dla obiektów pośredniczących Javy, reprezentujących
 * obiekty w bazie loxim. <br/>
 * W momencie wywołania metody na obiekcie pośredniczącym, wykonywana jest
 * odpowiednia operacja wybrana przez handler. 
 * Próba dostępu do atrybutów obiektu pośredniczącego spowoduje pobranie
 * lub modyfikacje odpowiednich wartości w bazie danych 
 * 
 * @author Tomasz Rosiek
 *
 */
public class JavaTransparentProxyHandler extends JavaBeanProxyInvocationHandler {
	
	private final Node node;
	
	private final ModelRegistry modelRegistry;
	
	private final TransparentProxyFactory transparentProxyFactory;
	
	private final Class entityClass;
	
	/**
	 * Domyślny konstruktor handlera
	 * @param node obiekt loxim który ma być reprezentowany 
	 * @param entityClass klasa Javy obiektu reprezentującego
	 * @param modelRegistry model danych
	 * @param transparentProxyFactory fabryka obiektów reprezentujących
	 */
	public JavaTransparentProxyHandler(
			final Node node, 
			final Class entityClass, 
			final ModelRegistry modelRegistry, 
			final TransparentProxyFactory transparentProxyFactory) {
		
		this.node = node;
		this.modelRegistry = modelRegistry;
		this.entityClass = entityClass;
		this.transparentProxyFactory = transparentProxyFactory;
		
	}
	
	@Override
	public Object invokeGetter(
			Object transparentProxy, 
			String propertyName, 
			Class returnType) {
				
		try {
			
			ClassInfo classInfo = modelRegistry.getClassInfo(entityClass);
			
			PropertyInfo propertyInfo = classInfo.getPropertyInfo(propertyName);
			
			Object result = propertyInfo.getTypeHandler().retrieveFromBase(
					node, 
					propertyName, 
					propertyInfo, 
					transparentProxyFactory);
			
			return result;			
		
		} catch (SBQLException e) {
			throw new RuntimeException(e);
		} 
		
	}

	@Override
	public Object invokeSetter(Object proxy, String propertyName, Object arg) {
		try {
		
			ClassInfo classInfo = modelRegistry.getClassInfo(entityClass);
			
			PropertyInfo propertyInfo = classInfo.getPropertyInfo(propertyName);
			
			propertyInfo.getTypeHandler().saveToBase(arg, node, propertyName, propertyInfo);
			
		} catch (SBQLException e) {
			throw new RuntimeException(e);
		}
		
		return null;
	}

	@Override
	public Object invokeEquals(Object proxy, Object arg) {
		if (!Proxy.isProxyClass(arg.getClass())) return false;
		
		JavaTransparentProxyHandler proxyHandler = 
			(JavaTransparentProxyHandler)Proxy.getInvocationHandler(proxy);
		
		InvocationHandler argHandler = Proxy.getInvocationHandler(arg);
		if (!(argHandler instanceof JavaTransparentProxyHandler)) return false;
		
		JavaTransparentProxyHandler castedArgHandler = 
			(JavaTransparentProxyHandler)argHandler;
		
		return (proxyHandler.node.getReference().equals(
				castedArgHandler.node.getReference()));
		
	}

	@Override
	public Object invokeHashCode(Object transparentProxy) {
		return node.getReference().hashCode();
	}

	public Node getNode() {
		return node;
	}

}
