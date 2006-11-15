package objectBrowser.model;

import java.util.LinkedList;
import java.util.List;

import loxim.pool.LoximQuery;

import org.eclipse.draw2d.geometry.Point;

public class ResultNode extends DiagramNode {
	protected List<DataObject> results = new LinkedList<DataObject>();
	protected LoximQuery query;
	public ResultNode(LoximQuery query) {
		super();
		this.query = query;
		setLocation(new Point(100, 100));
	}	
	
	public List<DataObject> getResults() {
		return results;
	}
	
	public void addResult(DataObject result){
		addResult(result, -1);
	}

	public void addResult(DataObject result, int index){
		if (index >= 0)
			results.add(index, result);
		else
			results.add(result);
		fireChildAdded(PROP_CHILDREN, result, new Integer(index));
	}		
	
	public LoximQuery getQuery() {
		return query;
	}
}
