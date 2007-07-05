package pl.tzr.transparent;

import java.lang.reflect.Proxy;
import java.util.HashMap;
import java.util.Map;

import pl.tzr.browser.store.node.Node;
import pl.tzr.test.data.Part;
import pl.tzr.transparent.proxy.JavaTransparentProxyHandler;

/**
 * Implementation of TransparentProxyFactory wich uses
 * Java Transparent Proxy
 * @author Tomasz Rosiek
 *
 */
public class JavaTransparentProxyFactory implements TransparentProxyFactory {
	
		
	private Map<Node, Object> proxies = new HashMap<Node, Object>();

	public JavaTransparentProxyFactory() {
		super();
	}

	public Object createRootProxy(Node node, TransparentSession session) {		
		Class desiredClass = getDesiredClass(node, session.getDatabaseContext()); 
		return createProxy(node, desiredClass, session);
	}	
	
	public Object createProxy(Node node, Class desiredClass, TransparentSession session) {
		
		if (proxies.containsKey(node)) {
			/* Proxy object has already been created in current session */
			return proxies.get(node);
		} else {
			/* Proxy object hasn't beed created - create it now */
			Object result = Proxy.newProxyInstance(
				    desiredClass.getClassLoader(),
				    new Class[] {desiredClass},
				    new JavaTransparentProxyHandler(node, desiredClass, session));
			proxies.put(node, result);
			return result;
		}
		
		
	}
	
	private Class getDesiredClass(Node proxy, DatabaseContext context) {
		return context.getModelRegistry().getClassInfo(proxy.getName()).getClazz();
	}

	public Node getNodeOfProxy(Object object) {
		JavaTransparentProxyHandler proxyHandler = 
			(JavaTransparentProxyHandler)Proxy.getInvocationHandler(object);
		
		return proxyHandler.getNode();		
	}

	
	public boolean isProxy(Object object) {
		return (Proxy.isProxyClass(object.getClass()));		
	}

}
