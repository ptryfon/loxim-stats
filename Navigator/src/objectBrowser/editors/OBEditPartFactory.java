package objectBrowser.editors;

import objectBrowser.model.CompositeDataObject;
import objectBrowser.model.Diagram;
import objectBrowser.model.ReferenceDataObject;
import objectBrowser.model.Relationship;
import objectBrowser.model.ResultNode;
import objectBrowser.model.SimpleDataObject;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;

public class OBEditPartFactory implements EditPartFactory {

	public EditPart createEditPart(EditPart context, Object model) {
		EditPart part = null;
		if (model instanceof Diagram) {
			part = new DiagramEditPart();
		} else if (model instanceof CompositeDataObject) {
			part = new CompositeDataObjectEditPart();
		} else if (model instanceof SimpleDataObject) {
			part = new SimpleDataObjectEditPart();
		} else if (model instanceof ReferenceDataObject) {
			part = new ReferenceDataObjectEditPart();			
		} else if (model instanceof ResultNode) {
			part = new ResultNodeEditPart();
		} else if (model instanceof Relationship) {
			part = new RelationEditPart();
		} 
		
		part.setModel(model);
		part.setParent(context);
		
		
		return part;
	}

}
