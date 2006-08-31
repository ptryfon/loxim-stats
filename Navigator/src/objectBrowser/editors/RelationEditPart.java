package objectBrowser.editors;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.ArrayList;
import java.util.List;

import javax.management.relation.Relation;

import objectBrowser.figures.OBFigureFactory;
import objectBrowser.model.RelationBendpoint;
import objectBrowser.model.Relationship;

import org.eclipse.draw2d.AbsoluteBendpoint;
import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ManhattanConnectionRouter;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.RelativeBendpoint;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractConnectionEditPart;

public class RelationEditPart extends AbstractConnectionEditPart implements PropertyChangeListener {
	
	public RelationEditPart() {
	}
	
	public void activate(){
		super.activate();
		getRelation().addPropertyChangeListener(this);
	}

	public void activateFigure(){
		super.activateFigure();
		/*Once the figure has been added to the ConnectionLayer, start listening for its
		 * router to change.
		 */
		getFigure().addPropertyChangeListener(Connection.PROPERTY_CONNECTION_ROUTER, this);
	}

	/**
	 * Adds extra EditPolicies as required. 
	 */
	protected void createEditPolicies() {
		installEditPolicy(EditPolicy.CONNECTION_ENDPOINTS_ROLE, new RelationBendpointEditPolicy());
		//Note that the Connection is already added to the diagram and knows its Router.
		refreshBendpointEditPolicy();
		installEditPolicy(EditPolicy.CONNECTION_ROLE,new RelationEditPolicy());
	}

	/**
	 * Returns a newly created Figure to represent the connection.
	 *
	 * @return  The created Figure.
	 */
	protected IFigure createFigure() {
		if (getRelation() == null)
			return null;
		Connection connx = OBFigureFactory.createRelationConnection(getRelation());
		return connx;
	}

	public void deactivate(){
		getRelation().removePropertyChangeListener(this);
		super.deactivate();
	}

	public void deactivateFigure(){
		getFigure().removePropertyChangeListener(Connection.PROPERTY_CONNECTION_ROUTER, this);
		super.deactivateFigure();
	}



	/**
	 * Returns the model of this represented as a Relation.
	 * 
	 * @return  Model of this as <code>Wire</code>
	 */
	protected Relationship getRelation() {
		return (Relationship)getModel();
	}

	/**
	 * Returns the Figure associated with this, which draws the
	 * Wire.
	 *
	 * @return  Figure of this.
	 */
	protected IFigure getWireFigure() {
		return (Connection) getFigure();
	}

	/**
	 * Listens to changes in properties of the Wire (like the
	 * contents being carried), and reflects is in the visuals.
	 *
	 * @param event  Event notifying the change.
	 */
	public void propertyChange(PropertyChangeEvent event) {
		String property = event.getPropertyName();
		if (Connection.PROPERTY_CONNECTION_ROUTER.equals(property)){
			refreshBendpoints();
			refreshBendpointEditPolicy();
		}
		if ("value".equals(property))   //$NON-NLS-1$
			refreshVisuals();
		if ("bendpoint".equals(property))   //$NON-NLS-1$
			refreshBendpoints();       
	}

	/**
	 * Updates the bendpoints, based on the model.
	 */
	protected void refreshBendpoints() {
		if (getConnectionFigure().getConnectionRouter() instanceof ManhattanConnectionRouter)
			return;
		List modelConstraint = getRelation().getBendpoints();
		List figureConstraint = new ArrayList();
		for (int i=0; i<modelConstraint.size(); i++) {
			RelationBendpoint wbp = (RelationBendpoint)modelConstraint.get(i);
			RelativeBendpoint rbp = new RelativeBendpoint(getConnectionFigure());
			rbp.setRelativeDimensions(wbp.getFirstRelativeDimension(),
										wbp.getSecondRelativeDimension());
			rbp.setWeight((i+1) / ((float)modelConstraint.size()+1));
			figureConstraint.add(rbp);
		}
		getConnectionFigure().setRoutingConstraint(figureConstraint);
	}

	private void refreshBendpointEditPolicy(){
		if (getConnectionFigure().getConnectionRouter() instanceof ManhattanConnectionRouter)
			installEditPolicy(EditPolicy.CONNECTION_BENDPOINTS_ROLE, null);
		else
			installEditPolicy(EditPolicy.CONNECTION_BENDPOINTS_ROLE, new RelationBendpointEditPolicy());
	}

	/**
	 * Refreshes the visual aspects of this, based upon the
	 * model (Wire). It changes the wire color depending on
	 * the state of Wire.
	 * 
	 */
	protected void refreshVisuals() {
		refreshBendpoints();
		
	}	
		

}
