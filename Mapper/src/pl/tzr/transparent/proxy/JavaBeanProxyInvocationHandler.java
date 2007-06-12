package pl.tzr.transparent.proxy;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

/**
 * Abstract implementation of Java Transparent Proxy InvocationHandler, 
 * destinated to use with JavaBean objects  
 * The following methods of the object are supported:
 * <ul>
 * 	<li>accessors (getXxx, setXxx, isXxx)</li>
 *  <li>equals</li>
 *  <li>hashCode</li>
 * </ul>
 * @author Tomasz Rosiek
 *
 */
public abstract class JavaBeanProxyInvocationHandler implements InvocationHandler {

	public Object invoke(Object proxy, Method m, Object[] args)
			throws Throwable {
		
		if (m.getName().startsWith("is")) {
			String propertyName = m.getName().substring(2);
			propertyName = Character.toLowerCase(propertyName.charAt(0)) + 
				propertyName.substring(1); 
					
			return invokeGetter(proxy, propertyName, m.getReturnType());

		} else if (m.getName().startsWith("get")) {

			String propertyName = m.getName().substring(3);
			propertyName = Character.toLowerCase(propertyName.charAt(0)) + 
				propertyName.substring(1); 
					
			return invokeGetter(proxy, propertyName, m.getReturnType());
			
			
		} else if (m.getName().startsWith("set")) {
			
			String propertyName = m.getName().substring(3);
			propertyName = Character.toLowerCase(propertyName.charAt(0)) + 
				propertyName.substring(1); 
					
			return invokeSetter(proxy, propertyName, args[0]);
			
		} else if (m.getName().equals("hashCode")) {
			
			return invokeHashCode(proxy); 
			
		} else if (m.getName().equals("equals")) {
			
			return invokeEquals(proxy, args[0]);
			
		} else if (m.getName().equals("toString")) {
			
			return invokeToString(proxy);
			
		} else throw new UnsupportedOperationException(m.getName()); 
		
	}
	
	/**
	 * Action ran when a getter method (getXxx or 
	 * isXxx) is executed on the proxy object
	 * @param proxy 
	 * 		proxy object
	 * @param propertyName 
	 * 		name of the attribute wich the getter concerns 
	 * @param returnType 
	 * 		expected result type
	 * @return
	 * 		getter execution result
	 */
	public abstract Object invokeGetter(Object proxy, String propertyName, 
			Class returnType);
	
	/**
	 * Action ran when a setter method (getXxx or 
	 * isXxx) is executed on the proxy object
	 * @param proxy 
	 * 		proxy object
	 * @param propertyName 
	 * 		name of the attribute wich the setter concerns
	 * @param arg argument wywoływanej metody
	 * 		argument of the setter
	 * @return
	 */
	public abstract Object invokeSetter(Object proxy, String propertyName, 
			Object arg);
	
	/**
	 * Action ran when a equals method is executed on the proxy object 
	 * @param proxy 
	 * 			proxy object
	 * @param arg 
	 *			argument of the proxy methods
	 * @return
	 * 		result of the equals method
	 */
	public abstract Object invokeEquals(Object proxy, Object arg);
	
	/**
	 * Action ran when a hashCode() method is executed on the proxy object 
	 * @param proxy 
	 * 			proxy object
	 * @return
	 * 		result of the hashCode method
	 */
	public abstract Object invokeHashCode(Object proxy);
	
	/**
	 * Action ran when a toString method is executed on the proxy object 
	 * @param proxy 
	 * 			proxy object
	 * @return
	 * 		result of the toString() method
	 */
	public abstract Object invokeToString(Object proxy);	

}
