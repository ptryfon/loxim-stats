package objectBrowser.editors;

import java.beans.PropertyChangeListener;
import java.util.LinkedList;
import java.util.List;

import objectBrowser.figures.CompositeDataObjectFigure;
import objectBrowser.figures.DataObjectFigure;
import objectBrowser.model.CompositeDataObject;
import objectBrowser.model.DataObject;
import objectBrowser.model.DataProperty;
import objectBrowser.model.Diagram;
import objectBrowser.model.DiagramNode;

import org.eclipse.core.runtime.Preferences.PropertyChangeEvent;
import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.DragTracker;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.NodeEditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.editpolicies.NonResizableEditPolicy;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;

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
