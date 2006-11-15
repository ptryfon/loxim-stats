package objectBrowser.editors;

import objectBrowser.figures.DataObjectFigure;
import objectBrowser.figures.ReferenceDataObjectFigure;
import objectBrowser.model.ReferenceDataObject;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.GraphicalEditPart;

public class ReferenceDataObjectEditPart extends DiagramElementEditPart   {
	

	@Override
	protected void refreshVisuals() {
	
	    ReferenceDataObject node = (ReferenceDataObject)getModel();
        ReferenceDataObjectFigure figure = (ReferenceDataObjectFigure)getFigure();
        figure.updateValue(node);
                        
        Rectangle r = new Rectangle(node.getLocation().x, node.getLocation().y, -1, -1);
        ((GraphicalEditPart)getParent()).setLayoutConstraint(this, figure, r);		
	}
	
	/**
	 * Returns the connection anchor for the given
	 * ConnectionEditPart's target. 
	 *
	 * @return  ConnectionAnchor.
	 */
	public ConnectionAnchor getTargetConnectionAnchor(ConnectionEditPart connEditPart) {
		return ((DataObjectFigure)getFigure()).getInputAnchor();
	}	

	@Override
	protected IFigure createFigure() {		
		return new ReferenceDataObjectFigure();
	}

	@Override
	protected void createEditPolicies() {
		//NonResizableEditPolicy nonResizable = new NonResizableEditPolicy();
		// TODO Auto-generated method stub

	}

	
	

}
