package objectBrowser.editors;

import objectBrowser.figures.OBFigureFactory;
import objectBrowser.figures.ResultNodeFigure;
import objectBrowser.model.DiagramNode;
import objectBrowser.model.ResultNode;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.GraphicalEditPart;

public class ResultNodeEditPart extends DiagramElementEditPart {

	@Override
	protected IFigure createFigure() {
		return new OBFigureFactory().createFigure(getDiagramPart());
	}
	
	protected DiagramNode getDiagramPart(){
		return (DiagramNode)getModel();
	}	
	

	@Override
	protected void createEditPolicies() {
		// TODO Auto-generated method stub

	}
	
	/**
	 * Returns the connection anchor for the given
	 * ConnectionEditPart's source. 
	 *
	 * @return  ConnectionAnchor.
	 */
	public ConnectionAnchor getSourceConnectionAnchor(ConnectionEditPart connEditPart) {
		return ((ResultNodeFigure)getFigure()).getOutputAnchor();
	}		

	@Override
	protected void refreshVisuals() {
		ResultNode node = (ResultNode)getModel();

                        
        Rectangle r = new Rectangle(node.getLocation().x, node.getLocation().y, -1, -1);
        ((GraphicalEditPart)getParent()).setLayoutConstraint(this, figure, r);		
	}	
}
