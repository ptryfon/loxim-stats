package objectBrowser.editors;

import java.beans.PropertyChangeListener;
import java.util.LinkedList;
import java.util.List;

import objectBrowser.figures.DataObjectFigure;
import objectBrowser.figures.SimpleDataObjectFigure;
import objectBrowser.model.CompositeDataObject;
import objectBrowser.model.DataObject;
import objectBrowser.model.DataProperty;
import objectBrowser.model.Diagram;
import objectBrowser.model.DiagramNode;
import objectBrowser.model.SimpleDataObject;

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

public class SimpleDataObjectEditPart extends DiagramElementEditPart   {
	

	




	@Override
	protected void refreshVisuals() {
	
	    SimpleDataObject node = (SimpleDataObject)getModel();
        SimpleDataObjectFigure figure = (SimpleDataObjectFigure)getFigure();
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
		return new SimpleDataObjectFigure();
	}

	@Override
	protected void createEditPolicies() {
		//NonResizableEditPolicy nonResizable = new NonResizableEditPolicy();
		// TODO Auto-generated method stub

	}

	
	

}
