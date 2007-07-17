package pl.tzr.transparent.proxy;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Proxy;
import java.util.HashMap;
import java.util.Map;

import pl.tzr.browser.store.node.Node;
import pl.tzr.driver.loxim.exception.SBQLException;
import pl.tzr.exception.NestedSBQLException;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.structure.model.ClassInfo;
import pl.tzr.transparent.structure.model.PropertyInfo;

/**
 * Implementation of the Java Transparent Proxy handler for Java object
 * representing LoXiM database objects
 * <br/>
 * When the methond on the proxy object is executed, then proper action
 * choosen by handler is ran.
 * Any access to attributes of the proxy objects will affect fetching
 * or updating proper values in the database
 * 
 * @author Tomasz Rosiek
 *
 */
public class JavaTransparentProxyHandler extends JavaBeanProxyInvocationHandler {
	
	private final Node node;
	
	private final TransparentSession session;
		
	private final Class entityClass;

	/* TODO - move it somewhere else */
	private final Map<String, Object> propertyValue = new HashMap<String, Object>();
	
	/**
	 * Default contstructor
	 * @param node 
	 * 			LoXiM node wich will be transparent proxied
	 * 	 
	 * @param entityClass
	 * 			Java class or interface wich the proxy node will implement 			
	 * @param modelRegistry 
	 * 			data model registry
	 * @param transparentProxyFactory 
	 *			factory of LoXiM transparent proxy objects
	 */
	public JavaTransparentProxyHandler(
			final Node node, 
			final Class entityClass,
			final TransparentSession session) {
		
		this.node = node;
		this.entityClass = entityClass;
		this.session = session;		
		
	}
	
	@Override
	public Object invokeGetter(
			Object transparentProxy, 
			String propertyName, 
			Class returnType) {
				
		try {
			
			ClassInfo classInfo = session.getDatabaseContext().getModelRegistry().getClassInfo(entityClass);
			
			PropertyInfo propertyInfo = classInfo.getPropertyInfo(propertyName);
			
			if (propertyInfo.isValueCacheable() && propertyValue.containsKey(propertyName)) {
				
				return propertyValue.get(propertyName);
				
			} else {
			
				Object result = propertyInfo.getAccessor().retrieveFromBase(
						node, 					 
						propertyInfo, session);
				
				if (propertyInfo.isValueCacheable()) propertyValue.put(propertyName, result);
				
				return result;
				
			}
									
		
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
		} 
		
	}

    @SuppressWarnings("unchecked")
	@Override
	public Object invokeSetter(Object proxy, String propertyName, Object arg) {
		try {
		
			ClassInfo classInfo = session.getDatabaseContext().getModelRegistry().getClassInfo(entityClass);			
			
			PropertyInfo propertyInfo = classInfo.getPropertyInfo(propertyName);
			            
			propertyInfo.getAccessor().saveToBase(arg, node, propertyInfo, session);
									
			if (propertyInfo.isValueCacheable())
				propertyValue.remove(propertyName);
						
		} catch (SBQLException e) {
			throw new NestedSBQLException(e);
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
	public Object invokeToString(Object proxy) {
		return node.getName() + "@" + node.getReference();
	}	

	@Override
	public Object invokeHashCode(Object transparentProxy) {
		return node.getReference().hashCode();
	}

	public Node getNode() {
		return node;
	}



}
