package objectBrowser.figures;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.graphics.Color;

public class ResultNodeFigure extends Ellipse{
	protected ConnectionAnchor outputAnchor;
	
	{
		FixedConnectionAnchor c;
		c = new FixedConnectionAnchor(this);
		c.offsetH = 20;
		outputAnchor = c;
		
		setSize(16, 16);
		setBackgroundColor(new Color(null, 128, 255, 128));
		setFill(true);
		setOpaque(true);
		setForegroundColor(new Color(null, 0, 0, 0));		
	}
	
	public ConnectionAnchor getOutputAnchor() {
		return outputAnchor;
	}
	 	
}
