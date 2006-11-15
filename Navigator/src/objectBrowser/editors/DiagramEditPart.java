package objectBrowser.editors;

import java.util.LinkedList;
import java.util.List;

import objectBrowser.model.Diagram;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FreeformLayout;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.gef.DragTracker;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.editpolicies.RootComponentEditPolicy;
import org.eclipse.gef.requests.SelectionRequest;
import org.eclipse.gef.tools.DeselectAllTracker;
import org.eclipse.gef.tools.MarqueeDragTracker;

public class DiagramEditPart extends AbstractGraphicalEditPart {

	@Override
	protected IFigure createFigure() {
		Figure f = new Figure();
		f.setOpaque(true);
		f.setLayoutManager(new FreeformLayout());
		f.setBorder(new MarginBorder(5));	
		return f;		
	}

	@Override
	protected void createEditPolicies() {
		installEditPolicy(EditPolicy.NODE_ROLE, null);
		installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE, null);
		installEditPolicy(EditPolicy.SELECTION_FEEDBACK_ROLE, null);
		installEditPolicy(EditPolicy.CONTAINER_ROLE, new DiagramContainerEditPolicy());
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new RootComponentEditPolicy());
		installEditPolicy(EditPolicy.LAYOUT_ROLE , new DiagramEditPolicy());
	}
	
	public DragTracker getDragTracker(Request req){
		if (req instanceof SelectionRequest 
			&& ((SelectionRequest)req).getLastButtonPressed() == 3)
				return new DeselectAllTracker(this);
		return new MarqueeDragTracker();
	}	

	@Override
	protected List getModelChildren() {
		List l = new LinkedList(((Diagram)getModel()).getObjects()); 
		return l;
	}

}
