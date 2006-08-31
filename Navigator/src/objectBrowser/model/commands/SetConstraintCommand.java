package objectBrowser.model.commands;

import objectBrowser.model.DiagramNode;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;

public class SetConstraintCommand extends Command {
	private Point newPos;
	private Dimension newSize;
	private Point oldPos;
	private Dimension oldSize;
	private DiagramNode part;

	public void execute() {
	        //oldSize = part.getSize();
	        oldPos  = part.getLocation();
	        redo();
	}

	public String getLabel() {
	        //if (oldSize.equals(newSize))
	                return "Move";
	        //return LogicMessages.SetLocationCommand_Label_Resize;
	}

	public void redo() {
	        //part.setSize(newSize);
	        part.setLocation(newPos);
	}

	public void setLocation(Rectangle r) {
	        setLocation(r.getLocation());
	        setSize(r.getSize());
	}

	public void setLocation(Point p) {
	        newPos = p;
	}

	public void setPart(DiagramNode part) {
	        this.part = part;
	}

	public void setSize(Dimension p) {
        newSize = p;
	}

	public void undo() {
	        //part.setSize(oldSize);
	        part.setLocation(oldPos);
	}
	
}
