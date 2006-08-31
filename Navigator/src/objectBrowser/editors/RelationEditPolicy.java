package objectBrowser.editors;

import objectBrowser.model.Relationship;
import objectBrowser.model.commands.DeleteConnetionCommand;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.ConnectionEditPolicy;
import org.eclipse.gef.requests.GroupRequest;

public class RelationEditPolicy extends ConnectionEditPolicy {

	@Override


	protected Command getDeleteCommand(GroupRequest request) {
		DeleteConnetionCommand c = new DeleteConnetionCommand();
		c.setRelationship((Relationship)getHost().getModel());
		return c;
	}


}
