package objectBrowser.editors;

import java.util.LinkedList;
import java.util.List;

import objectBrowser.figures.CompositeDataObjectFigure;
import objectBrowser.figures.DataObjectFigure;
import objectBrowser.model.CompositeDataObject;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.GraphicalEditPart;

public class CompositeDataObjectEditPart extends DiagramElementEditPart  {
	
	@Override
	public IFigure getContentPane() {
		return ((CompositeDataObjectFigure)getFigure()).getPropertiesCompartment();
	}

	
	@Override
	protected void refreshVisuals() {
	
	    CompositeDataObject node = (CompositeDataObject)getModel();
        CompositeDataObjectFigure figure = (CompositeDataObjectFigure)getFigure();
        figure.getName().setText(node.getName());                          

        Rectangle r = new Rectangle(node.getLocation().x, node.getLocation().y, -1, -1);
        ((GraphicalEditPart)getParent()).setLayoutConstraint(this, figure, r);		
	}

	@Override
	protected IFigure createFigure() {		
		return new CompositeDataObjectFigure();
	}

	@Override
	protected void createEditPolicies() {
		//NonResizableEditPolicy nonResizable = new NonResizableEditPolicy();
		// TODO Auto-generated method stub

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
	protected List getModelChildren() {
		List l = new LinkedList(((CompositeDataObject)getModel()).getChildren()); 
		return l;
	}	
}
