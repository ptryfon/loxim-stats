package objectBrowser.model.commands;

import java.util.Iterator;
import java.util.Vector;

import objectBrowser.model.DiagramNode;
import objectBrowser.model.Relationship;

import org.eclipse.gef.commands.Command;

public class DeleteConnetionCommand extends Command {
	
	protected DiagramNode oldSource;
	protected DiagramNode oldTarget;
	protected DiagramNode source;
	protected DiagramNode target; 
	protected Relationship relationship;	
	
	public boolean canExecute(){
		return true;
	}

	public void execute() {
		if (source != null){
			relationship.detachSource();
			relationship.setSource(source);
			relationship.attachSource();
		}
		if (target != null) {
			relationship.detachTarget();
			relationship.setTarget(target);
			relationship.attachTarget();
		}
		if (source == null && target == null){
			relationship.detachSource();
			relationship.detachTarget();
			relationship.setTarget(null);
			relationship.setSource(null);
		}
	}

	public String getLabel() {
		return "Connection command";
	}

	public DiagramNode getSource() {
		return source;
	}


	public DiagramNode getTarget() {
		return target;
	}


	public Relationship getRelationship() {
		return relationship;
	}

	public void redo() { 
		execute(); 
	}

	public void setSource(DiagramNode newSource) {
		source = newSource;
	}


	public void setTarget(DiagramNode newTarget) {
		target = newTarget;
	}


	public void setRelationship(Relationship w) {
		relationship = w;
		oldSource = w.getSource();
		oldTarget = w.getTarget();
	}

	public void undo() {
		source = relationship.getSource();
		target = relationship.getTarget();

		relationship.detachSource();
		relationship.detachTarget();

		relationship.setSource(oldSource);
		relationship.setTarget(oldTarget);

		relationship.attachSource();
		relationship.attachTarget();
	}

}
