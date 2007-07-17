package pl.tzr.transparent;

import pl.tzr.browser.store.node.Node;

/**
 * Implementations of this interface can create proxy objects representing
 * database nodes, and manage that proxies. 
 * @author Tomasz Rosiek
 *
 */
public interface TransparentProxyFactory {

	/**
	 * Creates Java Proxy object representing some database node
	 * @param node database node to be proxied
	 * @param desiredClass class or interface of created proxy object
	 * @param session connection sesion in wich the proxy object will be created
	 * @return proxy object representing node and implementing desiredClass 
	 * interface
	 */
	Object createProxy(Node node, Class desiredClass, TransparentSession session);
	
	Object createRootProxy(Node node, TransparentSession session);
	
	/**
	 * Returns true if provided object is node proxy 
	 * @param object potential proxy object
	 * @return true if object is node proxy
	 */
	boolean isProxy(Object object);
	
	/**
	 * Returns information about the node represented by provided proxy
	 * object
	 * @param object proxy object
	 * @return node represented by proxy object
	 */
	public Node getNodeOfProxy(Object object);
	
}
