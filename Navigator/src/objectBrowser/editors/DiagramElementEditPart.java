package objectBrowser.editors;

import java.beans.PropertyChangeListener;
import java.util.List;

import objectBrowser.model.DiagramNode;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.gef.DragTracker;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;

public abstract class DiagramElementEditPart extends AbstractGraphicalEditPart implements PropertyChangeListener {

	@Override
	public void activate(){
		if (isActive())
			return;
		super.activate();
		getDiagramPart().addPropertyChangeListener(this);
	}
	
	@Override
	/**
	 * Makes the EditPart insensible to changes in the model
	 * by removing itself from the model's list of listeners.
	 */
	public void deactivate(){
		if (!isActive())
			return;
		super.deactivate();
		getDiagramPart().removePropertyChangeListener(this);
	}
	
	/**
	 * Handles changes in properties of this. It is 
	 * activated through the PropertyChangeListener.
	 * It updates children, source and target connections,
	 * and the visuals of this based on the property
	 * changed.
	 *
	 * @param evt  Event which details the property change.
	 */
	public void propertyChange(java.beans.PropertyChangeEvent evt){
		String prop = evt.getPropertyName();
		if (DiagramNode.PROP_CHILDREN.equals(prop)) {
			if (evt.getOldValue() instanceof Integer)
				// new child
				addChild(createChild(evt.getNewValue()), ((Integer)evt
						.getOldValue()).intValue());
			else
				// remove child
				removeChild((EditPart)getViewer().getEditPartRegistry().get(
						evt.getOldValue()));
		}
/*		else if (LogicSubpart.INPUTS.equals(prop))
			refreshTargetConnections();
		else if (LogicSubpart.OUTPUTS.equals(prop))
			refreshSourceConnections(); */
		else if (prop.equals(DiagramNode.PROP_SIZE) || prop.equals(DiagramNode.PROP_LOCATION))
			refreshVisuals();
	}		

	@Override
	protected void createEditPolicies() {
		// TODO Auto-generated method stub

	}

	/**
	 * Returns the model associated with this as a DiagramPart.
	 *
	 * @return  The model of this as a LogicSubPart.
	 */	
	protected DiagramNode getDiagramPart(){
		return (DiagramNode)getModel();
	}
	
	@Override
	public DragTracker getDragTracker(Request arg0) {
		DragTracker d = super.getDragTracker(arg0); 
		return d;
	}
	
	/**
	 * Returns a list of connections for which this is the 
	 * source.
	 *
	 * @return List of connections.
	 */
	protected List getModelSourceConnections(){
		return getDiagramPart().getInputs();
	}

	/**
	 * Returns a list of connections for which this is the 
	 * target.
	 *
	 * @return  List of connections.
	 */
	protected List getModelTargetConnections(){
		return getDiagramPart().getOutputs();
	}
	
	
	/**
	 * Returns the connection anchor of a source connection which
	 * is at the given point.
	 * 
	 * @return  ConnectionAnchor.
	 */
	/*
	public ConnectionAnchor getSourceConnectionAnchor(Request request) {
		Point pt = new Point(((DropRequest)request).getLocation());
		return getNodeFigure().getSourceConnectionAnchorAt(pt);
	}
	*/

	
	/**
	 * Returns the connection anchor of a terget connection which
	 * is at the given point.
	 *
	 * @return  ConnectionAnchor.
	 */
	/*
	public ConnectionAnchor getTargetConnectionAnchor(Request request) {
		Point pt = new Point(((DropRequest)request).getLocation());
		return getNodeFigure().getTargetConnectionAnchorAt(pt);
	}
	*/

	/**
	 * Returns the name of the given connection anchor.
	 *
	 * @return  The name of the ConnectionAnchor as a String.
	 */
	final protected String mapConnectionAnchorToTerminal(ConnectionAnchor c){
		return "<NONE>";
	}	
}
