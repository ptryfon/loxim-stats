package pl.tzr.transparent.proxy;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

/**
 * Implementacja handlera dla mechanizmu Java Transparent Proxy, przeznaczona
 * do tworzenia obiektów pośredniczących zgodnych ze standardem JavaBeans. 
 * Obsługiwane jest działanie następujących metod
 * <ul>
 * 	<li>akcesory (getXxx, setXxx, isXxx)</li>
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
			
		} else throw new IllegalStateException("TODO");
		
	}
	
	/**
	 * Operacja wykonywana w momencie wywołania gettera (metody getXxx albo 
	 * isXxx) na obiekcie pośredniczącym proxy
	 * @param proxy obiekt pośredniczący
	 * @param propertyName nazwa atrybutu obiektu którego dotyczy wywołanie 
	 * @param returnType spodziewany typ wyniku
	 * @return
	 */
	public abstract Object invokeGetter(Object proxy, String propertyName, 
			Class returnType);
	
	/**
	 * Operacja wykonywana w momencie wywołania settera (metody getXxx albo 
	 * isXxx) na obiekcie pośredniczącym proxy
	 * @param proxy obiekt pośredniczący
	 * @param propertyName nazwa atrybutu obiektu którego dotyczy wywołanie
	 * @param arg argument wywoływanej metody
	 * @return
	 */
	public abstract Object invokeSetter(Object proxy, String propertyName, 
			Object arg);
	
	/**
	 * Operacja wykonywana w momencie wywołania metody equals na obiekcie 
	 * pośredniczącym proxy
	 * @param proxy obiekt pośredniczący
	 * @param arg argument metody equals
	 * @return
	 */
	public abstract Object invokeEquals(Object proxy, Object arg);
	
	/**
	 * Operacja wykonywana w momencie wywołania metody hashCode na obiekcie 
	 * pośredniczącym proxy
	 * @param proxy obiekt pośredniczący
	 * @return
	 */
	public abstract Object invokeHashCode(Object proxy);

}
