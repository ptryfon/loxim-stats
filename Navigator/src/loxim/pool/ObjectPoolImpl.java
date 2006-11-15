package loxim.pool;

import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class ObjectPoolImpl implements ObjectPool {
	List<LoximObject> objects = new LinkedList<LoximObject>();
	List<LoximQuery> queries = new LinkedList<LoximQuery>();
	Map<String, LoximObject> realObjects = new HashMap<String, LoximObject>();

	public void addObject(LoximObject object) {
		objects.add(object);
		if (object.getRef() != null) realObjects.put(object.getRef(), object);
		//TODO - moze przydaloby sie jakies powiadamianie kontenera o tym, ze zmienila sie referencja?
	}

	public boolean contains(String ref) {
		return realObjects.containsKey(ref);
	}

	public LoximObject getObject(String ref) {
		return realObjects.get(ref);
	}

	public Collection<LoximObject> getObjects() {
		return objects;
	}

	public String toString() {
		StringBuffer buf = new StringBuffer();
		boolean isFirst = true;
		for (LoximObject item : getObjects()) {
			if (isFirst) {
				isFirst = false;
			} else {
				buf.append("\n");
			}
			buf.append(item);
		}
		return buf.toString();
	}

	public void addQuery(LoximQuery query) {
		queries.add(query);
	}

	public Collection<LoximQuery> getQueries() {
		return queries;
	}
	
	
}
