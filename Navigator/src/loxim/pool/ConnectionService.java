package loxim.pool;

import java.util.Stack;

import loxim.driver.SimpleConnection;
import loxim.driver.exception.SBQLException;
import loxim.driver.result.Result;
import loxim.driver.result.ResultBag;

/**
 * Komponent realizujący operacje pobierania danych w oparciu o okreslone polaczenie z baza danych.
 * @author Tomasz Rosiek (tomasz.rosiek@gmail.com)
 *
 */
public class ConnectionService {
	protected ObjectPool pool;
	protected SimpleConnection connection;
	protected LoximObjectFactory factory;
	
	public ConnectionService(ObjectPool pool, SimpleConnection connection, LoximObjectFactory factory) {
		this.pool = pool;
		this.connection = connection;
		this.factory = factory;
	}
	
	/**
	 * Wykonuje zapytanie, umieszcza wynik w pudelku
	 * @param query
	 */
	public void executeQuery(String query) throws SBQLException{
		LoximQuery queryObject = new LoximQuery(query);
		Result result = connection.execute(query);
		if (result instanceof ResultBag) {
			ResultBag bag = (ResultBag)result;
			for (Result item : bag.getItems()) {
				LoximObject object = factory.buildObject(item);
				object.setQuery(queryObject);
				pool.addObject(object);
			}
		} else {
			LoximObject object = factory.buildObject(result);
			object.setQuery(queryObject);
			pool.addObject(object);
		}

		pool.addQuery(queryObject);
	}
	
	/**
	 * Pobiera obiekt wskazywany przez referencje
	 * @param ref
	 * @return pobrany obiekt
	 */
	public LoximObject fetchReferencedObject(String ref) throws SBQLException {
		if (!pool.contains(ref)) {
			Result result = connection.deref(ref);
			LoximObject obj = factory.buildObject(result);
			obj.setRef(ref);
			pool.addObject(obj);
			return obj;
		} else {
			return pool.getObject(ref);
		}
	}
	
	
	public void expandObject(Stack<LoximObject> stack, LoximObject obj) throws SBQLException {
		if (obj instanceof LoximSimpleObject) {
			/* nic nie robimy */
		} else if (obj instanceof LoximReference) {
			if (((LoximReference)obj).getTarget() == null) {
				((LoximReference)obj).fetchTarget(this);
				if (((LoximReference)obj).getTarget() != null) {
					stack.push(((LoximReference)obj).getTarget());
				} else {
					throw new RuntimeException("Cel nie znaleziony");
				}
			}
		} else if (obj instanceof LoximCompositeObject) {
			for (LoximObject item : ((LoximCompositeObject)obj).getProperties()) {
				stack.push(item);
			}
		} else throw new IllegalStateException("Nieoczekiwany rodzaj obiektu");
	}
	
	public void expandAll() throws SBQLException {
		Stack<LoximObject> awaitingObjects = new Stack<LoximObject>();
		awaitingObjects.addAll(pool.getObjects());
		
		while (!awaitingObjects.empty()) {
			LoximObject object = awaitingObjects.pop();
			expandObject(awaitingObjects, object);
		}

	}
}
