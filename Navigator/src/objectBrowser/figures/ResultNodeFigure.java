package objectBrowser.figures;

import objectBrowser.model.ResultNode;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Label;
import org.eclipse.swt.graphics.Color;

public class ResultNodeFigure extends Ellipse{
	protected ConnectionAnchor outputAnchor;
	protected Label toolTipLabel;
	
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
		
		toolTipLabel = new Label("Query");
		
		setToolTip(toolTipLabel);
	}
	
	public ConnectionAnchor getOutputAnchor() {
		return outputAnchor;
	}
	 	
	public void updateValue(ResultNode value) {
		toolTipLabel.setText("Query: " + value.getQuery().getValue());
	}
}
