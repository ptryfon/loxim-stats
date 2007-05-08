package pl.tzr.transparent;

import pl.tzr.browser.store.node.Node;

/**
 * Implementacje tego interfejsu są w stanie stworzyć obiekt Javy będący
 * reprezentacją wskazanego obiektu loxim
 * @author Tomasz Rosiek
 *
 */
public interface TransparentProxyFactory {

	/**
	 * Tworzy obiekt Javy reprezentujący określony obiekt loxim
	 * @param node obiekt loxim
	 * @param desiredClass oczekiwana klasa tworzonego obiektu
	 * @return
	 */
	Object createProxy(Node node, Class desiredClass);
	
	public Object createRootProxy(Node node);
	
	/**
	 * Zwraca true jeśli podany obiekt Javy jest reprezentacją obiektu loxim 
	 * @param object
	 * @return
	 */
	public boolean isProxy(Object object);
	
	/**
	 * Zwraca informacje o obiekcie loxim który jest reprezentowany przez
	 * podany w parametrze obiekt Javy
	 * @param object
	 * @return
	 */
	public Node getNodeOfProxy(Object object);
	
}
