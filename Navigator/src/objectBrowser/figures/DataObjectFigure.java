package objectBrowser.figures;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.Figure;

public class DataObjectFigure extends Figure {
	protected ConnectionAnchor inputAnchor;
	
	{
		FixedConnectionAnchor c;
		c = new FixedConnectionAnchor(this);
		c.offsetH = 20;
		inputAnchor = c;
	}
	
	public ConnectionAnchor getInputAnchor() {
		return inputAnchor;
	}
}
