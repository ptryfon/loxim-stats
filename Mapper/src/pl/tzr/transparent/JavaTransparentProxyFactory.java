package pl.tzr.transparent;

import java.lang.reflect.Proxy;
import java.util.HashMap;
import java.util.Map;

import pl.tzr.browser.store.node.Node;
import pl.tzr.test.data.Part;
import pl.tzr.transparent.proxy.JavaTransparentProxyHandler;
import pl.tzr.transparent.structure.model.ModelRegistry;

/**
 * Implementation of TransparentProxyFactory wich uses
 * Java Transparent Proxy
 * @author Tomasz Rosiek
 *
 */
public class JavaTransparentProxyFactory implements TransparentProxyFactory {
	
	private final ModelRegistry modelRegistry;
	
	private Map<Node, Object> proxies = new HashMap<Node, Object>();

	public JavaTransparentProxyFactory(final ModelRegistry modelRegistry) {
		super();
		this.modelRegistry = modelRegistry;
	}

	public Object createRootProxy(Node node) {		
		Class desiredClass = getDesiredClass(node); 
		return createProxy(node, desiredClass);
	}	
	
	public Object createProxy(Node node, Class desiredClass) {
		
		if (proxies.containsKey(node)) {
			/* Proxy object has already been created in current session */
			return proxies.get(node);
		} else {
			/* Proxy object hasn't beed created - create it now */
			Object result = Proxy.newProxyInstance(
				    desiredClass.getClassLoader(),
				    new Class[] {desiredClass},
				    new JavaTransparentProxyHandler(node, desiredClass, modelRegistry, this));
			proxies.put(node, result);
			return result;
		}
		
		
	}
	
	private Class getDesiredClass(Node proxy) {
		modelRegistry.getClassInfo(proxy.getName());
		return Part.class;
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
