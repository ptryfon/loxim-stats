package objectBrowser.model;

import loxim.pool.LoximCompositeObject;
import loxim.pool.LoximObject;
import loxim.pool.LoximQuery;
import loxim.pool.LoximReference;
import loxim.pool.LoximSimpleObject;
import loxim.pool.ObjectPool;

public class DiagramModelBuilder {

	ObjectPool pool;
	Diagram diagram;
	
	
	private DataObject buildObject(LoximObject obj) {
		if (obj instanceof LoximSimpleObject)
			return buildSimpleObject((LoximSimpleObject)obj);
		else if (obj instanceof LoximCompositeObject)
			return buildCompositeObject((LoximCompositeObject)obj);
		else if (obj instanceof LoximReference)
			return buildReferenceObject((LoximReference)obj);
		else throw new IllegalStateException("Nieoczekiwany rodzaj obiektu " + obj);
			
	}
	
	private DataObject buildSimpleObject(LoximSimpleObject obj) {
		return new SimpleDataObject(obj);
	}
	
	private DataObject buildCompositeObject(LoximCompositeObject obj) {
		CompositeDataObject result = new CompositeDataObject(obj);
		for (LoximObject child : obj.getProperties()) { 
			result.addChild(buildObject(child));
		}
		return result;
	}
	
	private DataObject buildReferenceObject(LoximReference obj) {
		ReferenceDataObject dataObject = new ReferenceDataObject(obj); 
		return dataObject;
	}
	
	private void makeRelation(DataObject object) {
		if (object.getTarget().getQuery() != null) {
			Relationship rel = new Relationship();
			ResultNode rn = diagram.getResultNode(object.getTarget().getQuery());
			rel.setSource(rn);
			rel.setTarget(object);
			rel.attachSource();
			rel.attachTarget();
		}
		if (object.getTarget() instanceof LoximReference) {
			
			LoximReference loximTarget = (LoximReference)(object.getTarget());
			if (loximTarget.getTarget() != null) {
				System.out.println("Source " + object);
				DataObject targetObject = diagram.getLoximOjbect(loximTarget.getTarget());
				System.out.println("Has target " + targetObject);
				Relationship rel = new Relationship();
				rel.setSource(object);
				rel.setTarget(targetObject);
				rel.attachSource();
				rel.attachTarget();
			}
			
		} else if (object instanceof CompositeDataObject) {
			for (DataObject childObject : ((CompositeDataObject)object).getChildren()) {
				makeRelation(childObject);
			}
		} else if (object.getTarget() instanceof LoximSimpleObject) {
			/* nic nie robimy */
		} else throw new IllegalStateException("Nieoczekiwany rodzaj obiektu " + object);
	}
	
	private void makeRelations() {
		for (DataObject obj : diagram.getLoximObjects()) {
			makeRelation(obj);
		}
	}
	
	public DiagramModelBuilder(ObjectPool pool) {
		this.pool = pool;
	}
	
	public Diagram makeModel() {
		diagram = new Diagram();		
		
		for (LoximQuery query : pool.getQueries()) {
			ResultNode rn = new ResultNode(query);
			diagram.addResultNode(query, rn);
		}
		
		for (LoximObject obj : pool.getObjects()) {
			DataObject diagramObj =  buildObject(obj);
			diagram.addLoximObject(obj, diagramObj);
	
		}
		
		makeRelations();
		
		return diagram;
	}
}
