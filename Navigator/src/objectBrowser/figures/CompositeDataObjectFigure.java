package objectBrowser.figures;


import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;

public class CompositeDataObjectFigure extends DataObjectFigure {
	  public static Color classColor = new Color(null,255,255,206);
	  private CompartmentFigure propertiesFigure = new CompartmentFigure();
	  private Label title;
	  
	  public CompositeDataObjectFigure() {
	    ToolbarLayout layout = new ToolbarLayout();
	    setLayoutManager(layout);	
	    setBorder(new LineBorder(ColorConstants.black,1));
	    setBackgroundColor(classColor);
	    setOpaque(true);
	    
	    title = new Label("<NAZWA>");
		Font titleFont = new Font(null, "Arial", 12, SWT.BOLD);
		title.setFont(titleFont);	    
		
	    add(title);	
	    add(propertiesFigure);
	  }
	  public CompartmentFigure getPropertiesCompartment() {
	    return propertiesFigure;
	  }
	public Label getName() {
		return title;
	}	
}
