
package objectBrowser.editors;

import objectBrowser.model.DiagramNode;
import objectBrowser.model.commands.SetConstraintCommand;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.XYLayoutEditPolicy;
import org.eclipse.gef.requests.CreateRequest;

public class DiagramEditPolicy extends XYLayoutEditPolicy {



	@Override
	protected EditPolicy createChildEditPolicy(EditPart arg0) {
		return new DiagramPartEditPolicy();
	}


	@Override
	protected Command createAddCommand(EditPart arg0, Object arg1) {
		// TODO Auto-generated method stub
		return null;
	}

	
	@Override
	protected Command createChangeConstraintCommand(EditPart child, Object constraint) {
		SetConstraintCommand command = new SetConstraintCommand();
		command.setPart((DiagramNode)child.getModel());
		command.setLocation((Rectangle)constraint);
		return command;		
	}

	@Override
	protected Command getCreateCommand(CreateRequest request) {
		/* Tworzenie nowego obiektu */
		return null;
	}

	@Override
	protected Command getDeleteDependantCommand(Request arg0) {
		return null;
	}

	@Override
	protected IFigure getFeedbackLayer() {
		return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
	}

}
