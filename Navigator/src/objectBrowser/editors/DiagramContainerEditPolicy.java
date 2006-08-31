package objectBrowser.editors;

import java.util.List;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.editpolicies.ContainerEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gef.requests.GroupRequest;

public class DiagramContainerEditPolicy extends ContainerEditPolicy {
	
	protected Command getCreateCommand(CreateRequest request) {
		return null;
	}

	public Command getOrphanChildrenCommand(GroupRequest request) {
		/*
		List parts = request.getEditParts();
		CompoundCommand result = 
			new CompoundCommand(LogicMessages.LogicContainerEditPolicy_OrphanCommandLabelText);
		for (int i = 0; i < parts.size(); i++) {
			OrphanChildCommand orphan = new OrphanChildCommand();
			orphan.setChild((LogicSubpart)((EditPart)parts.get(i)).getModel());
			orphan.setParent((LogicDiagram)getHost().getModel());
			orphan.setLabel(LogicMessages.LogicElementEditPolicy_OrphanCommandLabelText);
			result.add(orphan);
		}
		return result.unwrap();
		*/
		return null;
	}
	


}
