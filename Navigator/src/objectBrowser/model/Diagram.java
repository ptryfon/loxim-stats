package objectBrowser.model;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import loxim.pool.LoximObject;
import loxim.pool.LoximQuery;

public class Diagram {
	private Set<DiagramNode> objects = new HashSet<DiagramNode>();
	private Map<LoximObject, DataObject> loximObjects = new HashMap<LoximObject, DataObject>();
	private Map<LoximQuery, ResultNode> resultNodes = new HashMap<LoximQuery, ResultNode>();

	public Set<DiagramNode> getObjects() {
		return objects;
	}
	

	public DataObject getLoximOjbect(LoximObject obj) {
		return loximObjects.get(obj);
	}
	
	public ResultNode getResultNode(LoximQuery query) {
		return resultNodes.get(query);
	}
	
	public void addLoximObject(LoximObject obj, DataObject node) {
		loximObjects.put(obj, node);
		objects.add(node);
	}
	
	public void addResultNode(LoximQuery query, ResultNode node) {
		resultNodes.put(query, node);
		objects.add(node);
	}
	
	public Collection<DataObject> getLoximObjects() {
		return loximObjects.values();
	}
	
	public Collection<ResultNode> getResultNodes() {
		return resultNodes.values();
	}
}
