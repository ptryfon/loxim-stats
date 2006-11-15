package loxim.pool;

import java.util.Collection;

/**
 * Reprezentuje pudełko obiektów
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public interface ObjectPool {
	/**
	 * Zwraca zbiór wszystkich obiektów znajdujących się w pudełku
	 * @return
	 */
	public Collection<LoximObject> getObjects();
	
	/**
	 * Sprawdza, czy w pudełku znajduje sie obiekt o okreslonej referencji
	 * @param ref referencja
	 * @return prawda jesli obiekt jest w pudelku
	 */
	public boolean contains(String ref);
	
	/**
	 * Zwraca obiekt o określonej referencji. Jeśli obiekt nie istnieje zwraca null.
	 * @param ref
	 * @return obiekt
	 */
	public LoximObject getObject(String ref);
	
	/**
	 * Dodaje obiekt do pudełka
	 * @param object
	 */
	public void addObject(LoximObject object);
	
	public void addQuery(LoximQuery query);
	
	public Collection<LoximQuery> getQueries();
}
