package objectBrowser.figures;

import objectBrowser.model.ReferenceDataObject;
import objectBrowser.model.SimpleDataObject;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.swt.graphics.Color;

public class ReferenceDataObjectFigure extends DataObjectFigure {
	  public static Color classColor = new Color(null,255,255,206);
	  private Label valueLabel;
	  
	  public ReferenceDataObjectFigure() {
	    ToolbarLayout layout = new ToolbarLayout();
	    setLayoutManager(layout);		    	   
	    setOpaque(false);
	    setBorder(new MarginBorder(0, 2, 0, 2));
	    
	    valueLabel = new Label("<SIMPLE PROPERTY>");
		
	    add(valueLabel);	
	  }
	  
	  
	  
	  public void updateValue(ReferenceDataObject object) {
		  valueLabel.setText(object.getName() + "...");		
	  }
	  
}
